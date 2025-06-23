/**
 * @file irgen.c
 * @brief  递归遍历 AST, 生成 LLVM IR 文本缓冲区
 *
 * 仅本文件持有状态机 (IRg) 与所有 “generate_*” 递归函数。
 */

#include "helpers.h"
#include "ir.h"
#include "node.h"
#include "symbol.h"
#include "sysy.tab.h" /* token 宏 & AST Node 定义 */
#include "value.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct IRGenerator
{
    int reg_cnt;                  // 当前寄存器使用个数
    int label_cnt;                // 当前标签使用个数
    char **buf;                   // IR代码缓冲区
    int size;                     // 当前代码行数
    int cap;                      // 缓冲区容量
    Scope *current_scope;         // 当前作用域
    char *cur_ret_type;           // 当前函数的返回类型
    int last_is_terms;            // 上一个指令是否是终结指令 (如ret, br等)
    LoopLabels *loop_label_stack; // 指向循环标签栈的栈顶
} IRGen;

static IRGen g = {0};

/* ------------------------------------------------------------------ */
/**
 * @brief 生成一个新的 SSA 虚拟寄存器名。
 *
 * 生成规则：`%<递增计数>` ，计数器保存在全局 `g.reg_cnt` 中。
 *
 * @return `strdup` 后的寄存器字符串（调用方负责 `free`）
 *
 * @note
 *  - 函数使用了静态缓冲 `buf` 临时格式化，因此线程不安全。
 *  - 若寄存器大量增长，可适当增大 `buf` 长度。
 */
static char *new_reg(void)
{
    static char buf[16];
    sprintf(buf, "%%%d", ++g.reg_cnt); /* 计数器自增并格式化为 "%<n>" */
    return strdup(buf);                /* 复制一份供调用者使用      */
}

/* ------------------------------------------------------------------ */
/**
 * @brief 生成一个新的基本块标签。
 *
 * 例：`cond0` / `if_then17` / `while_body3`
 * 调用者提供前缀，内部使用 `g.label_cnt` 做递增后缀。
 *
 * @param prefix  标签前缀（不可为 NULL）
 * @return        `strdup` 后的标签字符串（调用方负责 `free`）
 */
static char *new_label(const char *prefix)
{
    static char buf[32];
    sprintf(buf, "%s%d", prefix, ++g.label_cnt);
    return strdup(buf);
}

//-----------------------------------------------------------------------------
//-----------------------辅助函数申明--------------------------------------------

char *generate_lval_address(Node *node);
char *build_llvm_type_str(Node *dim_node, const char *base_type);
void process_var_def_list(Node *n, const char *base_type);
void generate_initializer_stores(Node *init_node, char *base_ptr, char *llvm_type, int *dims, int num_dims,
                                 int *flat_index);
void get_dimensions(Node *dim_node, int *dims, int *num_dims);
void process_block_item_list(Node *n);
void process_param_types(Node *n, TypeNode **head_ref);
void process_formal_params(Node *n, int *count);
void process_actual_params(Node *n, Value *arg_vals[], int *count);
void emit(const char *fmt, ...);
//-----------------------------------------------------------------------------

static Value *generate_code(Node *node)
{
    if (!node || (node->name == NULL && node->num_children == 0))
        return NULL;

    if (strcmp(node->name, "CompUnit") == 0)
    {
        if (node->num_children > 0)
        {
            free_value(generate_code(node->children[0]));
            free_value(generate_code(node->children[1]));
        }
        return NULL;
    }
    else if (strcmp(node->name, "Block") == 0)
    {
        enter_scope();
        process_block_item_list(node->children[0]);
        exit_scope();
        return NULL;
    }
    else if (strcmp(node->name, "VarDecl") == 0 || strcmp(node->name, "ConstDecl") == 0)
    {
        char *type = get_type_str(node->children[0]);
        process_var_def_list(node->children[1], type);
        return NULL;
    }
    else if (strcmp(node->name, "FuncDef") == 0)
    {
        char *return_type_str = get_type_str(node->children[0]);
        char *func_name = node->children[1]->name;
        Node *params_opt_node = node->children[2];
        Node *block_node = node->children[3];
        TypeNode *params_list_head = NULL;
        if (params_opt_node->num_children > 0)
            process_param_types(params_opt_node->children[0], &params_list_head);

        char signature[1024] = ""; // 函数签名
        for (TypeNode *p = params_list_head; p; p = p->next)
        {
            strcat(signature, p->type);
            if (p->next)
                strcat(signature, ", ");
        }

        add_symbol_func(func_name, return_type_str, params_list_head);

        g.cur_ret_type = return_type_str;
        g.last_is_terms = 0;
        // 重置全局 IR 状态
        emit("\ndefine dso_local %s @%s(%s) #0 {", return_type_str, func_name, signature);
        emit("entry:");

        enter_scope();
        int param_count = 0;
        if (params_opt_node->num_children > 0)
        {
            count_params(params_opt_node->children[0], &param_count);
        }
        g.reg_cnt = param_count - 1; // 让寄存器编号从 %0 开始对齐

        param_count = 0; // 重置以进行实际处理。
        if (params_opt_node->num_children > 0)
        { // 形参搬运
            process_formal_params(params_opt_node->children[0], &param_count);
        }
        free_value(generate_code(block_node));
        exit_scope();

        if (!g.last_is_terms)
        {
            if (strcmp(return_type_str, "void") == 0)
                emit("\tret void");
            else
                emit("\tret %s 0", return_type_str);
        }
        emit("}");
        return NULL;
    }

    else if (strcmp(node->name, "ExpStmt") == 0)
    {
        if (node->num_children > 0 && node->children[0]->num_children > 0)
        {
            free_value(generate_code(node->children[0]->children[0]));
        }
        return NULL;
    }

    // --- Statements ---
    else if (strcmp(node->name, "AssignStmt") == 0)
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

    else if (strcmp(node->name, "ReturnStmt") == 0)
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

            if (strcmp(g.cur_ret_type, ret_val->type) != 0)
            {
                final_ret_reg = new_reg();
                if (strcmp(g.cur_ret_type, "float") == 0)
                    emit("\t%s = sitofp %s %s to float", final_ret_reg, ret_val->type, ret_val->reg);
                else
                    emit("\t%s = fptosi %s %s to i32", final_ret_reg, ret_val->type, ret_val->reg);
            }
            emit("\tret %s %s", g.cur_ret_type, final_ret_reg);
            if (final_ret_reg != ret_val->reg)
                free(final_ret_reg);
            free_value(ret_val);
        }
        else
        {
            emit("\tret void");
        }
        g.last_is_terms = 1;
        return NULL;
    }

    // --- Control Flow ---
    else if (strcmp(node->name, "IfStmt") == 0 || strcmp(node->name, "IfElseStmt") == 0 ||
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
            new_loop->next = g.loop_label_stack;
            // c. 更新栈顶指针
            g.loop_label_stack = new_loop;

            // 生成循环的进入和条件判断
            emit("\tbr label %%%s", cond_label);
            emit("%s:", cond_label);
            Value *cond_val = generate_code(node->children[0]);
            emit("\tbr i1 %s, label %%%s, label %%%s", cond_val->reg, body_label, after_label);
            free_value(cond_val);

            // 生成循环体
            emit("%s:", body_label);
            g.last_is_terms = 0; // 进入循环体前重置
            free_value(generate_code(node->children[1]));

            // 如果循环体内部没有 break/continue/return, 则跳回循环条件
            if (!g.last_is_terms)
            {
                emit("\tbr label %%%s", cond_label);
            }

            // 生成循环结束后的标签
            emit("%s:", after_label);
            g.last_is_terms = 0; // 循环结束后，代码流继续

            // --- 关键：出栈操作 ---
            LoopLabels *top = g.loop_label_stack;
            g.loop_label_stack = top->next; // 恢复到外层循环的栈
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
                g.last_is_terms = 0; // 重置标志进入 then 块
                free_value(generate_code(node->children[1]));
                // 独立为 then 分支添加跳转
                if (!g.last_is_terms)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                then_terminated = g.last_is_terms; // 记录 then 分支的状态

                // --- 处理 else 分支 ---
                emit("%s:", else_label);
                g.last_is_terms = 0; // 重置标志进入 else 块
                free_value(generate_code(node->children[2]));
                // 独立为 else 分支添加跳转
                if (!g.last_is_terms)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                else_terminated = g.last_is_terms; // 记录 else 分支的状态
                // merge 块只有在两个分支都未终结时才可能成为死代码，
                // 但无论如何都需要生成，所以不需要复杂的逻辑判断。
                // 关键是之后 last_is_terms 的状态。
                g.last_is_terms = then_terminated && else_terminated;
            }
            else // 处理 IfStmt (只有 then 分支)
            {
                // 对于 if(..){} 结构, else 分支就是直接到 merge_label
                emit("\tbr i1 %s, label %%%s, label %%%s", cond_val->reg, then_label, merge_label);

                // --- 处理 then 分支 ---
                emit("%s:", then_label);
                g.last_is_terms = 0; // 重置标志
                free_value(generate_code(node->children[1]));
                // 独立为 then 分支添加跳转
                if (!g.last_is_terms)
                {
                    emit("\tbr label %%%s", merge_label);
                }
                // if-then-else 的`else`分支相当于直接跳到merge,所以未终结
                g.last_is_terms = 0;
            }
        }

        emit("%s:", merge_label);
        // last_is_terms 的状态已经在 if/else 内部逻辑中正确设置
        // 如果是 if-else 且两个分支都 return 了, 这里才为 true, 否则都为 false。
        g.last_is_terms = 0;

        free_value(cond_val);
        free(then_label);
        free(else_label);
        free(merge_label);
        free(cond_label);
        free(body_label);
        return NULL;
    }
    else if (strcmp(node->name, "BreakStmt") == 0)
    {
        if (g.loop_label_stack)
        { // 检查是否在循环内
            // 跳转到栈顶的 "break_label"
            emit("\tbr label %%%s", g.loop_label_stack->break_label);
            g.last_is_terms = 1;
        }
        else
        {
            fprintf(stderr, "FATAL: 'break' statement not in a loop.\n");
            exit(1);
        }
        return NULL;
    }

    else if (strcmp(node->name, "ContinueStmt") == 0)
    {
        if (g.loop_label_stack)
        { // 检查是否在循环内
            // 跳转到栈顶的 "continue_label"
            emit("\tbr label %%%s", g.loop_label_stack->continue_label);
            g.last_is_terms = 1;
        }
        else
        {
            fprintf(stderr, "FATAL: 'continue' statement not in a loop.\n");
            exit(1);
        }
        return NULL;
    }

    // --- Expressions ---
    else if (strcmp(node->name, "BinaryOp") == 0)
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

            free_value(left);
            free_value(right);
            return new_value(res_reg, "i1"); // 逻辑运算的结果也是 i1
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
        free_value(left);
        free_value(right);
        return new_value(res_reg, result_type);
    }

    else if (strcmp(node->name, "UnaryOp") == 0)
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
            Value *res_val = new_value(res_reg, operand->type);
            free_value(operand);
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
            free_value(operand);
            return new_value(res_reg, "i32");
        }
    }
    else if (strcmp(node->name, "FuncCall") == 0)
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
            return_value = new_value(res_reg, func_sym->type);
        }

        // 5. 清理为参数动态分配的内存
        for (int i = 0; i < arg_count; i++)
        {
            free_value(arg_vals[i]);
        }

        // 返回函数调用的结果（对于 void 函数是 NULL，否则是一个新的 Value*）
        return return_value;
    }

    else if (strcmp(node->name, "InitVal_Aggregate") == 0)
        return new_value("0", "i32");

    // 数组或者是左值
    else if (strcmp(node->name, "ArrayAccess") == 0 || (strcmp(node->name, "LVal") == 0 && node->num_children == 1))
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
        return new_value(val_reg, element_type);
    }
    //  变量
    else if (node->num_children == 0)
    {
        Symbol *sym = lookup_symbol(node->name);
        if (sym && !sym->is_func)
        {
            // 普通变量
            char *val_reg = new_reg();
            emit("\t%s = load %s, %s* %s", val_reg, sym->type, sym->type, sym->reg_ptr);
            return new_value(val_reg, sym->type);
        }
        if (strchr(node->name, '.') || strchr(node->name, 'e') || strchr(node->name, 'E'))
        {
            // 浮点数字面量
            float f_val = (float)atof(node->name);
            union {
                float f;
                uint32_t u;
            } bits = {.f = f_val};
            char buf[64];
            snprintf(buf, sizeof(buf), "bitcast (i32 %u to float)", bits.u);
            return new_value(strdup(buf), "float");
        }
        // 整数字面量
        return new_value(node->name, "i32");
    }
    else if (node->num_children == 1)
        return generate_code(node->children[0]);
    return NULL;
}

//-----------------------------------------------------------------------------
//-----------------------辅助函数定义--------------------------------------------
//-----------------------------------------------------------------------------

/***********************************************************************
 *  LVal / 数组工具
 ***********************************************************************/

/* ------------------------------------------------------------------ */
/**
 * @brief 递归生成 “左值” 在栈上的地址（指针寄存器）。
 *
 * - `LVal`           ➜ 直接返回符号表里保存的 `reg_ptr`
 * - `ArrayAccess`    ➜ 先递归拿到基础指针，再 emit GEP
 *
 * @param node  `LVal` 或 `ArrayAccess` 语法节点
 * @return      新寄存器（strdup），调用者负责释放
 *
 * @warning 未检查数组越界；假设语义分析阶段已保证合法。
 */
char *generate_lval_address(Node *node)
{
    /* -------- 标识符变量 -------------------------------------- */
    if (strcmp(node->name, "LVal") == 0)
    {
        Symbol *sym = lookup_symbol(node->children[0]->name);
        if (sym)
            return strdup(sym->reg_ptr); /* 直接返回栈指针 */
    }

    /* -------- 数组访问：base[idx] ----------------------------- */
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        /* ① 递归求得基指针 */
        char *base_ptr = generate_lval_address(node->children[0]);

        /* ② 计算下标值（LLVM 要求先算好） */
        Value *index_val = generate_code(node->children[1]);

        /* ③ 获取聚合类型 `[N x T]`，用于 GEP 类型推导 */
        char *aggregate_type = get_lval_type(node->children[0]);

        /* ④ emit GEP 指令：&base[0][idx] … */
        char *res_ptr = new_reg();
        emit("\t%s = getelementptr inbounds %s, %s* %s, i32 0, %s %s", res_ptr, aggregate_type, aggregate_type,
             base_ptr, index_val->type, index_val->reg);

        /* ⑤ 清理临时资源并返回 */
        free(aggregate_type);
        free(base_ptr);
        free_value(index_val);
        return res_ptr;
    }

    /* -------- 非法调用 ---------------------------------------- */
    fprintf(stderr, "FATAL: generate_lval_address called with unexpected node '%s'\n", node->name);
    exit(1);
}

/* ------------------------------------------------------------------ */
/**
 * @brief 递归构造 LLVM 数组类型字符串。
 *
 * 语法树维度链形如：`dim_node -> [size] dim_node'`
 * 递归到最内层，把 `base_type` 包成 `[size x T]` 形式。
 *
 * @param dim_node   维度链起点 (`ArrDefDims` 之类)
 * @param base_type  元素类型 (`"i32" / "float"`)
 * @return           新 `strdup` 字符串（调用方负责释放）
 */
char *build_llvm_type_str(Node *dim_node, const char *base_type)
{
    /* 终止条件：无更多维度，直接返回基本类型 */
    if (!dim_node || dim_node->num_children == 0)
        return strdup(base_type);

    /*  递归处理剩余维度，先拿到内层类型 */
    char *inner_type = build_llvm_type_str(dim_node->children[1], base_type);

    /*  当前维度大小应为整型常量表达式 */
    Value *dim_val = generate_code(dim_node->children[0]);
    if (strcmp(dim_val->type, "i32") != 0)
    {
        fprintf(stderr, "FATAL: Array dimension must be an integer.\n");
        exit(1);
    }

    /*  拼装为 “[Size x Inner]” */
    char buf[512];
    sprintf(buf, "[%s x %s]", dim_val->reg, inner_type);

    /*  资源释放 */
    free(inner_type);
    free_value(dim_val);

    return strdup(buf);
}

/***********************************************************************
 *  变量定义 / 初值展开
 ***********************************************************************/

/* ------------------------------------------------------------------ */
/**
 * @brief 处理一个 **VarDef 列表**（可能链式递归）并生成 IR。
 *
 * 支持：
 *  - 基本类型 / 多维数组 (alloca + zeroinit)
 *  - 可选初始化（标量或聚合）
 *
 * @param n          `VarDefList` 节点
 * @param base_type  LLVM 基本类型字符串（如 `"i32"`/`"float"`)
 */
void process_var_def_list(Node *n, const char *base_type)
{
    if (!n || n->num_children == 0)
        return;

    /* ---------- 取出本次要处理的 VarDef ---------------------- */
    Node *var_def = n->children[0];
    char *var_name = var_def->children[0]->name;
    Node *dim_list_node = var_def->children[1];

    /* ----------  计算完整 LLVM 类型 ------------------------ */
    char *llvm_type = build_llvm_type_str(dim_list_node, base_type);

    /* ----------  分配栈槽并加入符号表 ---------------------- */
    char *ptr_reg = new_reg();
    emit("\t%s = alloca %s", ptr_reg, llvm_type);
    add_symbol(var_name, llvm_type, ptr_reg, /*is_const=*/0);

    /* ----------  对数组做零填充 ---------------------------- */
    if (dim_list_node->num_children > 0)
        emit("\tstore %s zeroinitializer, %s* %s", llvm_type, llvm_type, ptr_reg);

    /* ----------  处理可选初始化 ---------------------------- */
    if (strcmp(var_def->name, "VarDef_Init") == 0)
    {
        Node *init_val_node = var_def->children[2];

        if (dim_list_node->num_children > 0)
        { /* --------  数组初始化 (递归展开) --------------- */
            int dims[10] = {0}, num_dims = 0, flat_idx = 0;
            get_dimensions(dim_list_node, dims, &num_dims);
            generate_initializer_stores(init_val_node, ptr_reg, llvm_type, dims, num_dims, &flat_idx);
        }
        else
        { /* -------- 4.b 标量初始化 ------------------------- */
            Value *init_val = generate_code(init_val_node);
            char *final_reg = init_val->reg;

            /* 类型不一致时做隐式转换 */
            if (strcmp(base_type, init_val->type) != 0)
            {
                final_reg = new_reg();
                if (strcmp(base_type, "float") == 0)
                    emit("\t%s = sitofp %s %s to float", final_reg, init_val->type, init_val->reg);
                else
                    emit("\t%s = fptosi %s %s to i32", final_reg, init_val->type, init_val->reg);
            }

            emit("\tstore %s %s, %s* %s", base_type, final_reg, base_type, ptr_reg);

            if (final_reg != init_val->reg)
                free(final_reg);
            free_value(init_val);
        }
    }

    /* ---------- 5. 递归处理链表剩余部分 ---------------------- */
    free(llvm_type);
    free(ptr_reg);

    if (n->num_children > 1)
        process_var_def_list(n->children[1], base_type);
}

/* ------------------------------------------------------------------ */
/**
 * @brief （递归）将聚合初始化列表拆解成若干条 `store` 指令。
 *
 * @param init_node     `InitVal` / 嵌套 `InitVal_Aggregate` 节点
 * @param base_ptr      数组在栈上的基指针
 * @param llvm_type     完整数组类型字符串（形如 `[10 x i32]`）
 * @param dims          每一维大小数组
 * @param num_dims      维度个数
 * @param flat_index    扁平化下标（调用方持有，递归中递增）
 *
 * @note 本函数需与 `build_llvm_type_str` / `get_dimensions`
 *       保持一致的维度语义。
 */
void generate_initializer_stores(Node *init_node, char *base_ptr, char *llvm_type, int *dims, int num_dims,
                                 int *flat_index)
{
    /* ---------- 终止条件：标量 ------------------------------ */
    if (!init_node)
        return;

    if (is_scalar_init(init_node))
    {
        /*  计算子元素在数组中的多维下标 -> 构造 GEP index 列表 */
        Value *val = generate_code(init_node);

        char gep_idx[256] = "i32 0"; /* 前置 0：指向首元素 */
        {
            char tmp[32];
            int rem = *flat_index;
            for (int d = 0; d < num_dims; ++d)
            {
                int prod = 1;
                for (int k = d + 1; k < num_dims; ++k)
                    prod *= dims[k];
                int idx = rem / prod;
                sprintf(tmp, ", i32 %d", idx);
                strcat(gep_idx, tmp);
                rem %= prod;
            }
        }

        /*  得到元素指针并存值 */
        char *elem_ptr = new_reg();
        char elem_ty[8];
        strcpy(elem_ty, strstr(llvm_type, "float") ? "float" : "i32");

        char *final_reg = val->reg;
        if (strcmp(elem_ty, val->type) != 0)
        {
            final_reg = new_reg();
            if (strcmp(elem_ty, "float") == 0)
                emit("\t%s = sitofp i32 %s to float", final_reg, val->reg);
            else
                emit("\t%s = fptosi float %s to i32", final_reg, val->reg);
        }

        emit("\t%s = getelementptr inbounds %s, %s* %s, %s", elem_ptr, llvm_type, llvm_type, base_ptr, gep_idx);
        emit("\tstore %s %s, %s* %s", elem_ty, final_reg, elem_ty, elem_ptr);

        /*  清理 & flat_index++ */
        if (final_reg != val->reg)
            free(final_reg);
        free_value(val);
        free(elem_ptr);
        (*flat_index)++;
    }
    else
    {
        /* ---------- 递归：聚合 ------------------------------- */
        for (int i = 0; i < init_node->num_children; ++i)
            generate_initializer_stores(init_node->children[i], base_ptr, llvm_type, dims, num_dims, flat_index);
    }
}

/* ------------------------------------------------------------------ */
/*-----------------------------辅助递归函数-----------------------------*/
/* ------------------------------------------------------------------ */
/**
 * @brief 解析维度链表节点，收集每一维的常量大小。
 *
 * 语法形如 <dim> ::=
 * ```
 *   ConstExp
 *   ConstExp [更多维度]
 * ```
 *
 * 该函数将维度值写入 @p dims 数组，并累加维度个数 @p num_dims。
 * 依赖语义分析保证所有维度都是编译期常量整数。
 *
 * @param dim_node  AST 维度结点（形如 `[ConstExp]` 的链表）
 * @param dims      输出数组，保存每一维度大小
 * @param num_dims  已填入的维度数目（作为 in/out 索引）
 */
void get_dimensions(Node *dim_node, int *dims, int *num_dims)
{
    if (!dim_node || dim_node->num_children == 0)
        return; /* 递归终止 */

    /* 解析当前维度表达式 —— 返回值在 @c Value.reg 中是十进制字符串 */
    Value *dim_val = generate_code(dim_node->children[0]);
    dims[(*num_dims)++] = atoi(dim_val->reg); /* 写入并自增维度计数 */

    free_value(dim_val);

    /* 递归解析后续维度 */
    get_dimensions(dim_node->children[1], dims, num_dims);
}

/* -------------------------------------------------------------------------- */
/**
 * @brief 递归遍历并生成 <BlockItemList> 中的所有语句 / 声明。
 *
 * 语法结构：
 * ```
 * BlockItemList ::= BlockItem | BlockItemList BlockItem
 * ```
 * 该函数保证按源顺序生成 IR，并在离开节点时释放产生的临时值。
 *
 * @param node  AST BlockItemList 结点
 */
void process_block_item_list(Node *node)
{
    if (!node || node->num_children == 0)
        return;

    /* --- 1. 处理当前 BlockItem ---------------------------------- */
    free_value(generate_code(node->children[0])); /* 可能返回 Value* */

    /* --- 2. 递归处理剩余 BlockItem ------------------------------ */
    if (node->num_children > 1)
        process_block_item_list(node->children[1]);
}

/* -------------------------------------------------------------------------- */
/**
 * @brief 构建函数形参类型链表（反向递归实现正向顺序）。
 *
 * 该链表稍后用于生成函数签名以及存入符号表。
 *
 * @param n         AST ParamList / Param 结点
 * @param head_ref  指向链表头指针的指针（头插法）
 */
void process_param_types(Node *n, TypeNode **head_ref)
{
    if (!n || n->num_children == 0)
        return;

    /* 先递归到底，再头插 —— 最终链表顺序与源码形参顺序一致 */
    if (n->num_children > 1)
        process_param_types(n->children[1], head_ref);

    Node *param_node = n->children[0];
    TypeNode *tn = malloc(sizeof(TypeNode));
    tn->type = strdup(get_type_str(param_node->children[0]));
    tn->next = *head_ref;
    *head_ref = tn;
}

/**
 * @brief 在函数入口块为每个形参分配栈槽并写入符号表。
 *
 * @param n      AST ParamList / Param 结点
 * @param count  当前已处理的形参序号（作为寄存器编号）
 */
void process_formal_params(Node *n, int *count)
{
    if (!n || n->num_children == 0)
        return;

    /* ---------- 解析形参数目 1 个 -------------------------------- */
    Node *param_node = n->children[0];
    char *type_str = get_type_str(param_node->children[0]);
    char *name_str = param_node->children[1]->name;

    /*  形参在入口块先 `alloca` 再 `store %N` ------------------- */
    char arg_reg[16];
    snprintf(arg_reg, sizeof(arg_reg), "%%%d", (*count)++);
    char *reg_ptr = new_reg();
    emit("\t%s = alloca %s", reg_ptr, type_str);
    emit("\tstore %s %s, %s* %s", type_str, arg_reg, type_str, reg_ptr);

    /*  将形参加入当前作用域符号表 ------------------------------- */
    add_symbol(name_str, type_str, reg_ptr, /*is_const=*/0);

    free(reg_ptr); /* 仅释放临时字符串，本地变量仍指向符号表值 */

    /*  处理后续 “, param” -------------------------------------- */
    if (n->num_children > 1)
        process_formal_params(n->children[1], count);
}

/* -------------------------------------------------------------------------- */
/**
 * @brief 收集实参与其生成的临时寄存器，保持左到右顺序。
 *
 * @param n         AST FuncRParams / ExpList 结点
 * @param arg_vals  输出数组，保存生成的 Value*
 * @param count     已填入的实参与数量（递增）
 */
void process_actual_params(Node *n, Value *arg_vals[], int *count)
{
    if (!n || n->num_children == 0)
        return;

    /* 递归到最右端后回溯，以保证 arg_vals[0] 对应源码第一个实参 */
    if (n->num_children > 1)
        process_actual_params(n->children[1], arg_vals, count);

    arg_vals[(*count)++] = generate_code(n->children[0]);
}

//-----------------------------------------------------------------------------

// 生成器设置与销毁

static void init_ir_g()
{
    g.reg_cnt = 0;
    g.label_cnt = 0;
    g.size = 0;
    g.cap = 1024;
    g.buf = (char **)malloc(g.cap * sizeof(char *));
    g.current_scope = NULL;
    g.cur_ret_type = NULL;
    g.last_is_terms = 0;
    g.loop_label_stack = NULL;
    enter_scope();
}

static void free_ir_g()
{
    while (g.current_scope)
    {
        exit_scope();
    }
    for (int i = 0; i < g.size; i++)
    {
        free(g.buf[i]);
    }
    free(g.buf);
}

void emit(const char *fmt, ...)
{
    if (g.size == g.cap)
    {
        g.cap = g.cap ? g.cap << 1 : 1024;
        g.buf = realloc(g.buf, g.cap * sizeof(char *));
    }
    char line[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    g.buf[g.size++] = strdup(line);
}

static void write_ir_to_file(const char *filename)
{
    FILE *out = fopen(filename, "w");
    if (!out)
    {
        perror("Could not open output file");
        return;
    }
    for (int i = 0; i < g.size; i++)
    {
        fprintf(out, "%s\n", g.buf[i]);
    }
    fclose(out);
}

// 公共入口点

void generate_llvm_ir(struct Node *root, const char *out_path)
{
    memset(&g, 0, sizeof g);
    enter_scope();

    /* 递归遍历 AST */
    generate_code(root);
    FILE *fp = fopen(out_path, "w");
    for (int i = 0; i < g.size; i++)
        fprintf(fp, "%s\n", g.buf[i]);
    fclose(fp);
    /* 资源清理（作用域 / buf） */
    exit_scope();
    for (int i = 0; i < g.size; i++)
        free(g.buf[i]);
    free(g.buf);
}