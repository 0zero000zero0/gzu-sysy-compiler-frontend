#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"
#include "sysy.tab.h"

// 用于表示函数参数类型列表的节点
typedef struct TypeNode
{
    char *type;
    struct TypeNode *next;
} TypeNode;

typedef struct Value
{
    char *reg;
    char *type;
} Value;

static Value *newValue(const char *reg, const char *type)
{
    Value *v = (Value *)malloc(sizeof(Value));
    v->reg = strdup(reg);
    v->type = strdup(type);
    return v;
}

static void freeValue(Value *v)
{
    if (v)
    {
        free(v->reg);
        free(v->type);
        free(v);
    }
}

typedef struct Symbol
{
    char *name;
    char *type;     // 变量类型 或 函数返回类型
    char *llvm_reg; // 变量在栈上的地址指针
    int is_const;
    int is_func;      // 是否是函数
    TypeNode *params; // 函数参数类型列表
    struct Symbol *next;
} Symbol;

typedef struct Scope
{
    Symbol *head;         // 作用域内符号链表头
    struct Scope *parent; // 父作用域指针
} Scope;

typedef struct LoopLabels
{
    char *continue_label;    // continue 跳转的目标
    char *break_label;       // break 跳转的目标
    struct LoopLabels *next; // 指向外层循环的标签 (形成链表/栈)
} LoopLabels;

typedef struct
{
    int reg_counter;              // 当前寄存器使用个数
    int label_counter;            // 当前标签使用个数
    char **code_buffer;           // IR代码缓冲区
    int code_size;                // 当前代码行数
    int code_capacity;            // 缓冲区容量
    Scope *current_scope;         // 当前作用域
    char *current_func_ret_type;  // 当前函数的返回类型
    int last_instr_is_terminator; // 上一个指令是否是终结指令 (如ret, br等)
    LoopLabels *loop_label_stack; // 指向循环标签栈的栈顶
} IRGenerator;

static IRGenerator generator;

static char *get_type_str(Node *type_node);
static char *generate_lval_address(Node *node);
static void emit(const char *format, ...);
static void get_dimensions(Node *dim_node, int *dims, int *num_dims);
static void generate_initializer_stores(Node *init_node, char *base_ptr, char *llvm_type, int *dims, int num_dims,
                                        int *flat_index);
static void count_params(Node *n, int *count);
static int is_scalar_init(Node *n);


// 符号表管理
static void enter_scope()
{
    Scope *new_scope = (Scope *)malloc(sizeof(Scope));
    new_scope->head = NULL;
    new_scope->parent = generator.current_scope;
    generator.current_scope = new_scope;
}

static void exit_scope()
{
    if (generator.current_scope)
    {
        Scope *parent_scope = generator.current_scope->parent;
        Symbol *current = generator.current_scope->head;
        while (current)
        {
            Symbol *next = current->next;
            free(current->name);
            free(current->type);
            free(current->llvm_reg);
            free(current);
            current = next;
        }
        free(generator.current_scope);
        generator.current_scope = parent_scope;
    }
}

static void add_symbol_func(const char *name, const char *type, TypeNode *params)
{
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = strdup(type);
    new_symbol->llvm_reg = NULL; // 函数没有栈地址
    new_symbol->is_const = 0;
    new_symbol->is_func = 1;     // 标记为函数
    new_symbol->params = params; // 挂载参数类型列表
    new_symbol->next = generator.current_scope->head;
    generator.current_scope->head = new_symbol;
}

static void add_symbol(const char *name, const char *type, const char *llvm_reg, int is_const)
{
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = strdup(type);
    new_symbol->llvm_reg = strdup(llvm_reg);
    new_symbol->is_const = is_const;
    new_symbol->is_func = 0; // 标记为变量
    new_symbol->params = NULL;
    new_symbol->next = generator.current_scope->head;
    generator.current_scope->head = new_symbol;
}

static Symbol *lookup_symbol(const char *name)
{
    Scope *scope = generator.current_scope;
    while (scope)
    {
        Symbol *sym = scope->head;
        while (sym)
        {
            if (strcmp(sym->name, name) == 0)
            {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

static char *new_reg()
{
    static char buf[16];
    sprintf(buf, "%%%d", ++generator.reg_counter);
    return strdup(buf);
}

static char *new_label(const char *prefix)
{
    static char buf[32];
    sprintf(buf, "%s%d", prefix, ++generator.label_counter);
    return strdup(buf);
}

static char *get_btype(Node *btype_node)
{
    if (btype_node && btype_node->num_children > 0)
    {
        Node *type_token = btype_node->children[0];
        if (strcmp(type_token->name, "int") == 0)
            return "i32";
        if (strcmp(type_token->name, "float") == 0)
            return "float";
    }
    return "void";
}

static void process_param_types(Node *n, TypeNode **head_ref);
static void process_formal_params(Node *n, int *count);
static void process_actual_params(Node *n, Value *arg_vals[], int *count);
static void process_var_def_list(Node *n, const char *type);
static void process_block_item_list(Node *n);

static Value *generate_code(Node *node)
{
    if (!node || (node->name == NULL && node->num_children == 0))
        return NULL;

    if (strcmp(node->name, "CompUnit") == 0)
    {
        if (node->num_children > 0)
        {
            freeValue(generate_code(node->children[0]));
            freeValue(generate_code(node->children[1]));
        }
        return NULL;
    }
    if (strcmp(node->name, "Block") == 0)
    {
        enter_scope();
        process_block_item_list(node->children[0]);
        exit_scope();
        return NULL;
    }
    if (strcmp(node->name, "VarDecl") == 0 || strcmp(node->name, "ConstDecl") == 0)
    {
        char *type = get_type_str(node->children[0]);
        process_var_def_list(node->children[1], type);
        return NULL;
    }
    if (strcmp(node->name, "FuncDef") == 0)
    {
        char *return_type_str = get_type_str(node->children[0]);
        char *func_name = node->children[1]->name;
        Node *params_opt_node = node->children[2];
        Node *block_node = node->children[3];
        TypeNode *params_list_head = NULL;
        if (params_opt_node->num_children > 0)
            process_param_types(params_opt_node->children[0], &params_list_head);

        char signature[1024] = "";
        for (TypeNode *p = params_list_head; p; p = p->next)
        {
            strcat(signature, p->type);
            if (p->next)
                strcat(signature, ", ");
        }

        add_symbol_func(func_name, return_type_str, params_list_head);
        generator.current_func_ret_type = return_type_str;
        generator.last_instr_is_terminator = 0;

        emit("\ndefine dso_local %s @%s(%s) #0 {", return_type_str, func_name, signature);
        emit("entry:");

        enter_scope();
        int param_count = 0;
        if (params_opt_node->num_children > 0)
            count_params(params_opt_node->children[0], &param_count);

        generator.reg_counter = param_count - 1;

        param_count = 0; // 重置以进行实际处理。
        if (params_opt_node->num_children > 0)
            process_formal_params(params_opt_node->children[0], &param_count);

        freeValue(generate_code(block_node));
        exit_scope();

        if (!generator.last_instr_is_terminator)
        {
            if (strcmp(return_type_str, "void") == 0)
                emit("\tret void");
            else
                emit("\tret %s 0", return_type_str);
        }
        emit("}");
        return NULL;
    }

    if (strcmp(node->name, "ExpStmt") == 0)
    {
        if (node->num_children > 0 && node->children[0]->num_children > 0)
        {
            freeValue(generate_code(node->children[0]->children[0]));
        }
        return NULL;
    }

    // --- Statements ---
    if (strcmp(node->name, "AssignStmt") == 0)
    {
        Value *rhs_val = generate_code(node->children[1]);
        char *lhs_ptr_reg = generate_lval_address(node->children[0]);

        Node *base_var_node = node->children[0];
        while (strcmp(base_var_node->name, "ArrayAccess") == 0)
            base_var_node = base_var_node->children[0];
        Symbol *lhs_sym = lookup_symbol(base_var_node->children[0]->name);

        char lhs_type[64];
        if (lhs_sym && strstr(lhs_sym->type, "float"))
            strcpy(lhs_type, "float");
        else
            strcpy(lhs_type, "i32");

        char *final_rhs_reg = rhs_val->reg;
        if (strcmp(lhs_type, rhs_val->type) != 0)
        {
            final_rhs_reg = new_reg();
            if (strcmp(lhs_type, "float") == 0)
                emit("\t%s = sitofp i32 %s to float", final_rhs_reg, rhs_val->reg);
            else
                emit("\t%s = fptosi float %s to i32", final_rhs_reg, rhs_val->reg);
        }

        emit("\tstore %s %s, %s* %s", lhs_type, final_rhs_reg, lhs_type, lhs_ptr_reg);

        // 释放此函数内部分配的资源
        free(lhs_ptr_reg);
        if (final_rhs_reg != rhs_val->reg)
        {
            free(rhs_val->reg);           // 释放旧的寄存器名
            rhs_val->reg = final_rhs_reg; // 更新为新的寄存器名
            // 注意：这里我们假设 rhs_val->type 也需要更新，但赋值表达式的值类型应与左值匹配
            free(rhs_val->type);
            rhs_val->type = strdup(lhs_type);
        }

        // 将 rhs_val 的所有权转移给调用者，不再在此处释放
        return rhs_val;
    }

    if (strcmp(node->name, "ReturnStmt") == 0)
    {
        if (node->children[0]->num_children > 0)
        {
            Value *ret_val = generate_code(node->children[0]->children[0]);
            // 增加健壮性检查
            if (!ret_val)
            {
                fprintf(stderr, "FATAL: Expression in return statement produced no value.\n");
                exit(1);
            }
            char *final_ret_reg = ret_val->reg;

            if (strcmp(generator.current_func_ret_type, ret_val->type) != 0)
            {
                final_ret_reg = new_reg();
                if (strcmp(generator.current_func_ret_type, "float") == 0)
                    emit("\t%s = sitofp %s %s to float", final_ret_reg, ret_val->type, ret_val->reg);
                else
                    emit("\t%s = fptosi %s %s to i32", final_ret_reg, ret_val->type, ret_val->reg);
            }
            emit("\tret %s %s", generator.current_func_ret_type, final_ret_reg);
            if (final_ret_reg != ret_val->reg)
                free(final_ret_reg);
            freeValue(ret_val);
        }
        else
        {
            emit("\tret void");
        }
        generator.last_instr_is_terminator = 1;
        return NULL;
    }

    // --- Control Flow ---
    if (strcmp(node->name, "IfStmt") == 0 || strcmp(node->name, "IfElseStmt") == 0 ||
        strcmp(node->name, "WhileStmt") == 0)
    {
        char *then_label = NULL, *else_label = NULL, *merge_label = NULL, *cond_label = NULL, *body_label = NULL;
        Value *cond_val;
        int then_terminated, else_terminated; // 用于记录各分支是否已终结
        if (strcmp(node->name, "WhileStmt") == 0)
        {
            // 1. 为当前循环创建标签
            char *cond_label = new_label("while_cond");
            char *body_label = new_label("while_body");
            char *after_label = new_label("while_after");

            // --- 关键：入栈操作 ---
            // a. 创建新的标签节点
            LoopLabels *new_loop = (LoopLabels *)malloc(sizeof(LoopLabels));
            new_loop->continue_label = strdup(cond_label);
            new_loop->break_label = strdup(after_label);
            // b. 将新节点链接到旧的栈顶
            new_loop->next = generator.loop_label_stack;
            // c. 更新栈顶指针
            generator.loop_label_stack = new_loop;

            // 生成循环的进入和条件判断
            emit("\tbr label %%%s", cond_label);
            emit("%s:", cond_label);
            Value *cond_val = generate_code(node->children[0]);
            emit("\tbr i1 %s, label %%%s, label %%%s", cond_val->reg, body_label, after_label);
            freeValue(cond_val);

            // 生成循环体
            emit("%s:", body_label);
            generator.last_instr_is_terminator = 0; // 进入循环体前重置
            freeValue(generate_code(node->children[1]));

            // 如果循环体内部没有 break/continue/return, 则跳回循环条件
            if (!generator.last_instr_is_terminator)
            {
                emit("\tbr label %%%s", cond_label);
            }

            // 生成循环结束后的标签
            emit("%s:", after_label);
            generator.last_instr_is_terminator = 0; // 循环结束后，代码流继续

            // --- 关键：出栈操作 ---
            LoopLabels *top = generator.loop_label_stack;
            generator.loop_label_stack = top->next; // 恢复到外层循环的栈
            free(top->continue_label);
            free(top->break_label);
            free(top);

            // 释放为这个循环创建的标签
            free(cond_label);
            free(body_label);
            free(after_label);
            return NULL;
        }
        else
        {
            // 处理 IfStmt 和 IfElseStmt
            cond_val = generate_code(node->children[0]);
            then_label = new_label("if_then");
            merge_label = new_label("if_merge");

            if (strcmp(node->name, "IfElseStmt") == 0)
            {
                else_label = new_label("if_else");
                emit("\tbr i1 %s, label %%%s, label %%%s", cond_val->reg, then_label, else_label);

                // --- 处理 then 分支 ---
                emit("%s:", then_label);
                generator.last_instr_is_terminator = 0; // 重置标志进入 then 块
                freeValue(generate_code(node->children[1]));
                // 独立为 then 分支添加跳转
                if (!generator.last_instr_is_terminator)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                then_terminated = generator.last_instr_is_terminator; // 记录 then 分支的状态

                // --- 处理 else 分支 ---
                emit("%s:", else_label);
                generator.last_instr_is_terminator = 0; // 重置标志进入 else 块
                freeValue(generate_code(node->children[2]));
                // 独立为 else 分支添加跳转
                if (!generator.last_instr_is_terminator)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                else_terminated = generator.last_instr_is_terminator; // 记录 else 分支的状态
                // merge 块只有在两个分支都未终结时才可能成为死代码，
                // 但无论如何都需要生成，所以不需要复杂的逻辑判断。
                // 关键是之后 last_instr_is_terminator 的状态。
                generator.last_instr_is_terminator = then_terminated && else_terminated;
            }
            else // 处理 IfStmt (只有 then 分支)
            {
                // 对于 if(..){} 结构, else 分支就是直接到 merge_label
                emit("\tbr i1 %s, label %%%s, label %%%s", cond_val->reg, then_label, merge_label);

                // --- 处理 then 分支 ---
                emit("%s:", then_label);
                generator.last_instr_is_terminator = 0; // 重置标志
                freeValue(generate_code(node->children[1]));
                // 独立为 then 分支添加跳转
                if (!generator.last_instr_is_terminator)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                // if-then-else 的`else`分支相当于直接跳到merge,所以未终结
                generator.last_instr_is_terminator = 0;
            }
        }

        emit("%s:", merge_label);
        // last_instr_is_terminator 的状态已经在 if/else 内部逻辑中正确设置
        // 如果是 if-else 且两个分支都 return 了, 这里才为 true, 否则都为 false。
        generator.last_instr_is_terminator = 0;

        freeValue(cond_val);
        free(then_label);
        free(else_label);
        free(merge_label);
        free(cond_label);
        free(body_label);
        return NULL;
    }
    if (strcmp(node->name, "BreakStmt") == 0)
    {
        if (generator.loop_label_stack)
        { // 检查是否在循环内
            // 跳转到栈顶的 "break_label"
            emit("\tbr label %%%s", generator.loop_label_stack->break_label);
            generator.last_instr_is_terminator = 1;
        }
        else
        {
            fprintf(stderr, "FATAL: 'break' statement not in a loop.\n");
            exit(1);
        }
        return NULL;
    }

    if (strcmp(node->name, "ContinueStmt") == 0)
    {
        if (generator.loop_label_stack)
        { // 检查是否在循环内
            // 跳转到栈顶的 "continue_label"
            emit("\tbr label %%%s", generator.loop_label_stack->continue_label);
            generator.last_instr_is_terminator = 1;
        }
        else
        {
            fprintf(stderr, "FATAL: 'continue' statement not in a loop.\n");
            exit(1);
        }
        return NULL;
    }

    // --- Expressions ---
    if (strcmp(node->name, "BinaryOp") == 0)
    {
        Value *left = generate_code(node->children[1]);
        Value *right = generate_code(node->children[2]);
        char *op_str = node->children[0]->name;
        if (strcmp(op_str, "&&") == 0 || strcmp(op_str, "||") == 0)
        {
            // 逻辑运算的操作数必须是 i1 类型
            // TODO: 假设它们已经是 i1，如果不是，需要先转换
            char *op_code = (strcmp(op_str, "&&") == 0) ? "and" : "or";
            char *res_reg = new_reg();

            // LLVM 的 'and'/'or' 指令用于 i1 类型的布尔运算
            emit("\t%s = %s i1 %s, %s", res_reg, op_code, left->reg, right->reg);

            freeValue(left);
            freeValue(right);
            return newValue(res_reg, "i1"); // 逻辑运算的结果也是 i1
        }

        char op_code_buf[16], *result_type = "i32", *op_prefix = "";
        int is_cmp = 0;

        if (strcmp(left->type, "float") == 0 || strcmp(right->type, "float") == 0)
        {
            result_type = "float";
            op_prefix = "f";
        }
        char *left_reg = left->reg;
        if (strcmp(result_type, "float") == 0 && strcmp(left->type, "i32") == 0)
        {
            left_reg = new_reg();
            emit("\t%s = sitofp i32 %s to float", left_reg, left->reg);
        }
        char *right_reg = right->reg;
        if (strcmp(result_type, "float") == 0 && strcmp(right->type, "i32") == 0)
        {
            right_reg = new_reg();
            emit("\t%s = sitofp i32 %s to float", right_reg, right->reg);
        }

        if (strcmp(op_str, "+") == 0)
            sprintf(op_code_buf, "%sadd", op_prefix);
        else if (strcmp(op_str, "-") == 0)
            sprintf(op_code_buf, "%ssub", op_prefix);
        else if (strcmp(op_str, "*") == 0)
            sprintf(op_code_buf, "%smul", op_prefix);
        else if (strcmp(op_str, "/") == 0)
            sprintf(op_code_buf, "%sdiv", op_prefix);
        else if (strcmp(op_str, "%") == 0)
            sprintf(op_code_buf, "srem");
        else
        {
            is_cmp = 1;
            if (strcmp(result_type, "float") == 0)
            {
                if (strcmp(op_str, "==") == 0)
                    strcpy(op_code_buf, "oeq");
                else if (strcmp(op_str, "!=") == 0)
                    strcpy(op_code_buf, "one");
                else if (strcmp(op_str, "<") == 0)
                    strcpy(op_code_buf, "olt");
                else if (strcmp(op_str, ">") == 0)
                    strcpy(op_code_buf, "ogt");
                else if (strcmp(op_str, "<=") == 0)
                    strcpy(op_code_buf, "ole");
                else if (strcmp(op_str, ">=") == 0)
                    strcpy(op_code_buf, "oge");
            }
            else
            {
                if (strcmp(op_str, "==") == 0)
                    strcpy(op_code_buf, "eq");
                else if (strcmp(op_str, "!=") == 0)
                    strcpy(op_code_buf, "ne");
                else if (strcmp(op_str, "<") == 0)
                    strcpy(op_code_buf, "slt");
                else if (strcmp(op_str, ">") == 0)
                    strcpy(op_code_buf, "sgt");
                else if (strcmp(op_str, "<=") == 0)
                    strcpy(op_code_buf, "sle");
                else if (strcmp(op_str, ">=") == 0)
                    strcpy(op_code_buf, "sge");
            }
        }
        char *res_reg = new_reg();
        if (is_cmp)
        {
            char *cmp_instr = (strcmp(result_type, "float") == 0) ? "fcmp" : "icmp";
            emit("\t%s = %s %s %s %s, %s", res_reg, cmp_instr, op_code_buf, result_type, left_reg, right_reg);
            result_type = "i1";
        }
        else
        {
            emit("\t%s = %s %s %s, %s", res_reg, op_code_buf, result_type, left_reg, right_reg);
        }
        if (left_reg != left->reg)
            free(left_reg);
        if (right_reg != right->reg)
            free(right_reg);
        freeValue(left);
        freeValue(right);
        return newValue(res_reg, result_type);
    }

    if (strcmp(node->name, "UnaryOp") == 0)
    {
        Value *operand = generate_code(node->children[1]);
        char *op_str = node->children[0]->name;
        if (strcmp(op_str, "+") == 0)
            return operand;
        if (strcmp(op_str, "-") == 0)
        {
            char *res_reg = new_reg();
            if (strcmp(operand->type, "float") == 0)
                emit("\t%s = fsub float 0.0, %s", res_reg, operand->reg);
            else
                emit("\t%s = sub i32 0, %s", res_reg, operand->reg);
            Value *res_val = newValue(res_reg, operand->type);
            freeValue(operand);
            return res_val;
        }
        if (strcmp(op_str, "!") == 0)
        {
            char *cmp_reg = new_reg();
            if (strcmp(operand->type, "float") == 0)
                emit("\t%s = fcmp oeq float %s, 0.0", cmp_reg, operand->reg);
            else
                emit("\t%s = icmp eq i32 %s, 0", cmp_reg, operand->reg);
            char *res_reg = new_reg();
            emit("\t%s = zext i1 %s to i32", res_reg, cmp_reg);
            free(cmp_reg);
            freeValue(operand);
            return newValue(res_reg, "i32");
        }
    }

    if (strcmp(node->name, "FuncCall") == 0)
    {
        char *func_name = node->children[0]->name;
        Symbol *func_sym = lookup_symbol(func_name);

        if (!func_sym || !func_sym->is_func)
        {
            fprintf(stderr, "FATAL: Call to undefined or non-function '%s'.\n", func_name);
            exit(1);
        }

        // 处理实际参数
        Value *arg_vals[32]; // 假设最多32个参数
        int arg_count = 0;
        Node *params_node = node->children[1]; // FuncRParams 节点

        // 如果存在参数节点 (FuncRParams) 且它有子节点 (ExpList)
        if (params_node && params_node->num_children > 0)
        {
            Node *exp_list_node = params_node->children[0];
            // 循环遍历表达式列表 (ExpList)
            while (exp_list_node && exp_list_node->num_children > 0)
            {
                // 为每个参数表达式生成代码，并将结果存入数组
                arg_vals[arg_count++] = generate_code(exp_list_node->children[0]);

                // 移动到下一个 ExpList 节点（如果存在）
                if (exp_list_node->num_children > 1)
                {
                    exp_list_node = exp_list_node->children[1];
                }
                else
                {
                    break;
                }
            }
        }

        // 3. 构建LLVM call指令的参数字符串
        char args_str[2048] = "";
        // 注意：这里的实现未做详细的实参-形参类型检查与隐式转换，
        // 仅使用生成代码时的类型
        for (int i = 0; i < arg_count; i++)
        {
            char temp_buf[128];
            sprintf(temp_buf, "%s %s", arg_vals[i]->type, arg_vals[i]->reg);
            strcat(args_str, temp_buf);
            if (i < arg_count - 1)
            {
                strcat(args_str, ", ");
            }
        }

        // 4. 根据函数返回类型，生成 call 指令并返回相应的值
        Value *return_value = NULL;
        if (strcmp(func_sym->type, "void") == 0)
        {
            // 如果是 void 函数，call 指令没有返回值
            emit("\tcall void @%s(%s)", func_name, args_str);
            // void 函数调用的值是 NULL
            return_value = NULL;
        }
        else
        {
            // 如果有返回值，call 指令的结果存入一个新寄存器
            char *res_reg = new_reg();
            emit("\t%s = call %s @%s(%s)", res_reg, func_sym->type, func_name, args_str);
            // 创建一个新的 Value* 来代表函数调用的结果
            return_value = newValue(res_reg, func_sym->type);
        }

        // 5. 清理为参数动态分配的内存
        for (int i = 0; i < arg_count; i++)
        {
            freeValue(arg_vals[i]);
        }

        // 返回函数调用的结果（对于 void 函数是 NULL，否则是一个新的 Value*）
        return return_value;
    }

    if (strcmp(node->name, "InitVal_Aggregate") == 0)
        return newValue("0", "i32");

    // --- Primitives (LVal, Literals) ---
    if (strcmp(node->name, "ArrayAccess") == 0 || (strcmp(node->name, "LVal") == 0 && node->num_children == 1))
    {
        char *ptr_reg = generate_lval_address(node);
        char element_type[64];
        Node *base_var_node = node;
        while (strcmp(base_var_node->name, "ArrayAccess") == 0)
            base_var_node = base_var_node->children[0];
        Symbol *sym = lookup_symbol(base_var_node->children[0]->name);
        if (sym && strstr(sym->type, "float"))
            strcpy(element_type, "float");
        else
            strcpy(element_type, "i32");

        char *val_reg = new_reg();
        emit("\t%s = load %s, %s* %s", val_reg, element_type, element_type, ptr_reg);
        free(ptr_reg);
        return newValue(val_reg, element_type);
    }

    if (node->num_children == 0)
    {
        Symbol *sym = lookup_symbol(node->name);
        if (sym && !sym->is_func)
        {
            char *val_reg = new_reg();
            emit("\t%s = load %s, %s* %s", val_reg, sym->type, sym->type, sym->llvm_reg);
            return newValue(val_reg, sym->type);
        }
        if (strchr(node->name, '.') || strchr(node->name, 'e') || strchr(node->name, 'E'))
        {
            double float_val = atof(node->name);
            char hex_float_buf[128];
            sprintf(hex_float_buf, "%f", float_val);
            return newValue(hex_float_buf, "float");
        }
        return newValue(node->name, "i32");
    }

    if (node->num_children == 1)
        return generate_code(node->children[0]);
    return NULL;
}

/* ================================================================== */
/* 辅助函数 (Helpers)                                           */
/* ================================================================== */
// get_type_str: 从类型节点获取LLVM类型字符串
static char *get_type_str(Node *type_node)
{
    if (strcmp(type_node->name, "int") == 0)
        return "i32";
    if (strcmp(type_node->name, "float") == 0)
        return "float";
    return "void";
}

// ir.c: 在 process_formal_params 附近添加这个新函数
static void count_params(Node *n, int *count)
{
    if (!n || n->num_children == 0)
        return;
    (*count)++;
    if (n->num_children > 1)
    {
        count_params(n->children[1], count);
    }
}

// generate_lval_address: 为左值(LVal)生成地址
static char *get_lval_type(Node *node)
{
    if (strcmp(node->name, "LVal") == 0)
    {
        Symbol *sym = lookup_symbol(node->children[0]->name);
        return strdup(sym->type);
    }
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        char *base_type = get_lval_type(node->children[0]);
        // 从聚合类型 [N x T] 中推断出元素类型 T
        char *element_type = strchr(base_type, 'x');
        if (element_type)
        {
            char result[256];
            // element_type+2 跳过 "x "
            strcpy(result, element_type + 2);
            // 去掉末尾的 ']'
            result[strlen(result) - 1] = '\0';
            free(base_type);
            return strdup(result);
        }
        return base_type;
    }
    return NULL;
}

static char *generate_lval_address(Node *node)
{
    if (strcmp(node->name, "LVal") == 0)
    {
        Symbol *sym = lookup_symbol(node->children[0]->name);
        if (sym)
            return strdup(sym->llvm_reg);
    }

    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        char *base_ptr_reg = generate_lval_address(node->children[0]);
        Value *index_val = generate_code(node->children[1]);

        // 递归地获取基指针指向的正确聚合类型
        char *aggregate_type = get_lval_type(node->children[0]);

        char *res_ptr_reg = new_reg();
        emit("\t%s = getelementptr inbounds %s, %s* %s, i32 0, %s %s", res_ptr_reg, aggregate_type, aggregate_type,
             base_ptr_reg, index_val->type, index_val->reg);

        free(aggregate_type);
        free(base_ptr_reg);
        freeValue(index_val);
        return res_ptr_reg;
    }

    fprintf(stderr, "FATAL: generate_lval_address called with unexpected node type '%s'.\n", node->name);
    exit(1);
}

static char *build_llvm_type_str(Node *dim_node, const char *base_type)
{
    if (!dim_node || dim_node->num_children == 0)
    {
        return strdup(base_type);
    }
    char *inner_type = build_llvm_type_str(dim_node->children[1], base_type);

    Value *dim_size_val = generate_code(dim_node->children[0]);
    if (strcmp(dim_size_val->type, "i32") != 0)
    {
        fprintf(stderr, "FATAL: Array dimension must be an integer.\n");
        exit(1);
    }

    char result_buf[512];
    sprintf(result_buf, "[%s x %s]", dim_size_val->reg, inner_type);

    free(inner_type);
    freeValue(dim_size_val);

    return strdup(result_buf);
}

// process_var_def_list: 处理变量定义列表

static void process_var_def_list(Node *n, const char *base_type)
{
    if (!n || n->num_children == 0)
        return;

    Node *var_def_node = n->children[0];
    char *var_name = var_def_node->children[0]->name;
    Node *dim_list_node = var_def_node->children[1];
    char *llvm_type = build_llvm_type_str(dim_list_node, base_type);
    char *ptr_reg = new_reg();

    emit("\t%s = alloca %s", ptr_reg, llvm_type);
    add_symbol(var_name, llvm_type, ptr_reg, 0);

    if (dim_list_node->num_children > 0)
    {
        emit("\tstore %s zeroinitializer, %s* %s", llvm_type, llvm_type, ptr_reg);
    }

    if (strcmp(var_def_node->name, "VarDef_Init") == 0)
    {
        Node *init_val_node = var_def_node->children[2];
        if (dim_list_node->num_children > 0)
        { // 数组初始化
            int dims[10] = {0}, num_dims = 0;
            get_dimensions(dim_list_node, dims, &num_dims);
            int flat_index = 0;

            // --- 关键修正 ---
            // 直接将 InitVal 节点 (其 name 可能是 InitVal_Aggregate) 传递下去
            // generate_initializer_stores 函数内部知道如何处理它
            generate_initializer_stores(init_val_node, ptr_reg, llvm_type, dims, num_dims, &flat_index);
        }
        else
        { // 简单变量初始化
            Value *init_val = generate_code(init_val_node);
            char *final_init_reg = init_val->reg;

            if (strcmp(base_type, init_val->type) != 0)
            {
                final_init_reg = new_reg();
                if (strcmp(base_type, "float") == 0)
                {
                    emit("\t%s = sitofp %s %s to float", final_init_reg, init_val->type, init_val->reg);
                }
                else
                {
                    emit("\t%s = fptosi %s %s to i32", final_init_reg, init_val->type, init_val->reg);
                }
            }
            emit("\tstore %s %s, %s* %s", base_type, final_init_reg, base_type, ptr_reg);

            if (final_init_reg != init_val->reg)
                free(final_init_reg);
            freeValue(init_val);
        }
    }

    free(llvm_type);
    free(ptr_reg);

    if (n->num_children > 1)
    {
        process_var_def_list(n->children[n->num_children - 1], base_type);
    }
}

// get_dimensions: 从维度AST节点中提取维度信息
static void get_dimensions(Node *dim_node, int *dims, int *num_dims)
{
    if (!dim_node || dim_node->num_children == 0)
        return;

    Value *dim_val = generate_code(dim_node->children[0]);
    dims[(*num_dims)++] = atoi(dim_val->reg);
    freeValue(dim_val);

    get_dimensions(dim_node->children[1], dims, num_dims);
}

// generate_initializer_stores: 生成初始化存储指令

static void generate_initializer_stores(Node *init_node, char *base_ptr, char *llvm_type, int *dims, int num_dims,
                                        int *flat_index)
{
    // 如果节点为空，直接返回
    if (!init_node)
        return;

    // 基准情形: 节点是一个最终的数值表达式
    if (is_scalar_init(init_node))
    {
        Value *val = generate_code(init_node);

        char gep_indices[256] = "i32 0";
        char temp[32];
        int temp_index = *flat_index;
        for (int i = 0; i < num_dims; i++)
        {
            int dim_prod = 1;
            for (int j = i + 1; j < num_dims; j++)
                dim_prod *= dims[j];
            int index_i = temp_index / dim_prod;
            sprintf(temp, ", i32 %d", index_i);
            strcat(gep_indices, temp);
            temp_index %= dim_prod;
        }

        char *element_ptr = new_reg();
        char element_type[64];
        strcpy(element_type, (strstr(llvm_type, "float") ? "float" : "i32"));

        char *final_val_reg = val->reg;
        if (strcmp(element_type, val->type) != 0)
        {
            final_val_reg = new_reg();
            if (strcmp(element_type, "float") == 0)
                emit("\t%s = sitofp i32 %s to float", final_val_reg, val->reg);
            else
                emit("\t%s = fptosi float %s to i32", final_val_reg, val->reg);
        }

        emit("\t%s = getelementptr inbounds %s, %s* %s, %s", element_ptr, llvm_type, llvm_type, base_ptr, gep_indices);
        emit("\tstore %s %s, %s* %s", element_type, final_val_reg, element_type, element_ptr);

        if (final_val_reg != val->reg)
            free(final_val_reg);
        freeValue(val);
        free(element_ptr);
        (*flat_index)++;
    }
    // 递归情形: 节点是任何类型的包装器或列表
    else
    {
        for (int i = 0; i < init_node->num_children; ++i)
            generate_initializer_stores(init_node->children[i], base_ptr, llvm_type, dims, num_dims, flat_index);
    }
}

// process_block_item_list: 递归处理语句块内条目
static void process_block_item_list(Node *n)
{
    if (!n || n->num_children == 0)
        return;

    freeValue(generate_code(n->children[0]));

    if (n->num_children > 1)
    {
        process_block_item_list(n->children[1]);
    }
}

// 修正：健壮的参数类型列表构建函数
static void process_param_types(Node *n, TypeNode **head_ref)
{
    if (!n || n->num_children == 0)
        return;
    // 先递归，再头插，实现链表反转，得到正确顺序
    if (n->num_children > 1)
        process_param_types(n->children[1], head_ref);
    Node *param_node = n->children[0];
    TypeNode *new_tn = (TypeNode *)malloc(sizeof(TypeNode));
    new_tn->type = strdup(get_type_str(param_node->children[0]));
    new_tn->next = *head_ref;
    *head_ref = new_tn;
}

// 修正：健壮的函数形参处理函数
static void process_formal_params(Node *n, int *count)
{
    if (!n || n->num_children == 0)
        return;
    Node *param_node = n->children[0];
    char *param_type = get_type_str(param_node->children[0]);
    char *param_name = param_node->children[1]->name;
    char param_reg[16];
    snprintf(param_reg, sizeof(param_reg), "%%%d", (*count)++);
    char *ptr_reg = new_reg();
    emit("\t%s = alloca %s", ptr_reg, param_type);
    emit("\tstore %s %s, %s* %s", param_type, param_reg, param_type, ptr_reg);
    add_symbol(param_name, param_type, ptr_reg, 0);
    free(ptr_reg);
    if (n->num_children > 1)
        process_formal_params(n->children[1], count);
}

// 修正：健壮的函数实参处理函数
static void process_actual_params(Node *n, Value *arg_vals[], int *count)
{
    if (!n || n->num_children == 0)
        return;

    // 递归处理列表的剩余部分，以保持参数顺序正确
    if (n->num_children > 1)
    {
        process_actual_params(n->children[1], arg_vals, count);
    }
    // 处理当前节点的值
    arg_vals[(*count)++] = generate_code(n->children[0]);
}

// 生成器设置与销毁

static void init_ir_generator()
{
    generator.reg_counter = 0;
    generator.label_counter = 0;
    generator.code_size = 0;
    generator.code_capacity = 1024;
    generator.code_buffer = (char **)malloc(generator.code_capacity * sizeof(char *));
    generator.current_scope = NULL;
    generator.current_func_ret_type = NULL;
    generator.last_instr_is_terminator = 0;
    generator.loop_label_stack = NULL;
    enter_scope();
}

static void free_ir_generator()
{
    while (generator.current_scope)
    {
        exit_scope();
    }
    for (int i = 0; i < generator.code_size; i++)
    {
        free(generator.code_buffer[i]);
    }
    free(generator.code_buffer);
}

static void emit(const char *format, ...)
{
    if (generator.code_size >= generator.code_capacity)
    {
        // 扩容
        generator.code_capacity *= 2;
        generator.code_buffer = (char **)realloc(generator.code_buffer, generator.code_capacity * sizeof(char *));
    }
    char temp_buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(temp_buf, sizeof(temp_buf), format, args);
    va_end(args);
    generator.code_buffer[generator.code_size++] = strdup(temp_buf);
}

static void write_ir_to_file(const char *filename)
{
    FILE *out = fopen(filename, "w");
    if (!out)
    {
        perror("Could not open output file");
        return;
    }
    for (int i = 0; i < generator.code_size; i++)
    {
        fprintf(out, "%s\n", generator.code_buffer[i]);
    }
    fclose(out);
}

static int is_scalar_init(Node *n)
{
    /* 叶子节点或明确的 ConstExp / Exp 都视为一个可直接求值的标量 */
    return (n->num_children == 0) || strcmp(n->name, "ConstExp") == 0 || strcmp(n->name, "Exp") == 0;
}
// 公共入口点

void generate_llvm_ir(Node *root, const char *output_filename)
{
    init_ir_generator();
    generate_code(root);
    write_ir_to_file(output_filename);
    free_ir_generator();
}