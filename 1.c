#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"
#include "sysy.tab.h"

// --- 数据结构定义 ---
// TypeNode, Symbol, Scope, IRGenerator 的定义与上一版相同
// 此处省略以保持简洁，请确保你的文件中保留了这些定义

// --- 全局生成器实例 ---
static IRGenerator generator;

// --- 函数前向声明 ---
static char *generate_code(Node *node);
static char *generate_lval_address(Node *node);
static void init_ir_generator();
static void free_ir_generator();
static void emit(const char *format, ...);
static void process_block_item_list(Node *n);
static void process_var_def_list(Node *n, const char *type);
static void process_param_types(Node *n, TypeNode **head_ref);
static void process_formal_params(Node *n, int *count);
static void process_actual_params(Node *n, char *arg_reg_list[], int *count);
// ... 其他必要的函数声明 ...

/* ================================================================== */
/* 核心代码：适配新文法的 generate_code 函数             */
/* ================================================================== */

static char *generate_code(Node *node)
{
    if (!node || (node->name == NULL && node->num_children == 0))
        return NULL;

    // --- 1. 顶层结构与作用域 ---
    if (strcmp(node->name, "CompUnit") == 0)
    {
        if (node->num_children > 0)
        {
            generate_code(node->children[0]); // 处理 Decl 或 FuncDef
            generate_code(node->children[1]); // 递归处理列表的剩余部分
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

    // --- 2. 声明 ---
    if (strcmp(node->name, "VarDecl") == 0 || strcmp(node->name, "ConstDecl") == 0)
    {
        char *type_name = node->children[0]->name;
        char *type = strcmp(type_name, "int") == 0 ? "i32" : "float";
        process_var_def_list(node->children[1], type); // 使用辅助函数处理VarDef列表
        return NULL;
    }

    // --- 3. 函数定义 ---
    if (strcmp(node->name, "FuncDef") == 0)
    {
        char *type_name = node->children[0]->name;
        char *func_name = node->children[1]->name;
        Node *params_opt_node = node->children[2];
        Node *block_node = node->children[3];
        char *return_type_str = strcmp(type_name, "void") == 0 ? "void" : "i32";

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
            process_formal_params(params_opt_node->children[0], &param_count);
        generate_code(block_node);
        exit_scope();

        if (!generator.last_instr_is_terminator)
        {
            if (strcmp(return_type_str, "void") == 0)
                emit("  ret void");
            else
                emit("  ret %s 0", return_type_str);
        }
        emit("}");
        return NULL;
    }

    // --- 4. 语句 ---
    if (strcmp(node->name, "AssignStmt") == 0)
    {
        char *val_reg = generate_code(node->children[1]);
        char *ptr_reg = generate_lval_address(node->children[0]); // 获取左值地址
        Symbol *sym = lookup_symbol(node->children[0]->name);     // 仅为获取类型
        if (sym)
            emit("  store %s %s, %s* %s", sym->type, val_reg, sym->type, ptr_reg);
        free(val_reg);
        free(ptr_reg);
        return NULL;
    }
    if (strcmp(node->name, "ReturnStmt") == 0)
    {
        Node *exp_opt_node = node->children[0];
        if (exp_opt_node->num_children > 0)
        {
            char *ret_val_reg = generate_code(exp_opt_node->children[0]);
            emit("  ret %s %s", generator.current_func_ret_type, ret_val_reg);
            free(ret_val_reg);
        }
        else
        {
            emit("  ret void");
        }
        generator.last_instr_is_terminator = 1;
        return NULL;
    }
    if (strcmp(node->name, "IfElseStmt") == 0 || strcmp(node->name, "IfStmt") == 0)
    {
        char *cond_reg = generate_code(node->children[0]);
        char *i1_cond_reg = new_reg();
        emit("  %s = icmp ne i32 %s, 0", i1_cond_reg, cond_reg);
        free(cond_reg);

        char *then_label = new_label("if_then"), *else_label = new_label("if_else"), *merge_label = new_label("if_merge");

        if (strcmp(node->name, "IfElseStmt") == 0)
        {
            emit("  br i1 %s, label %%%s, label %%%s", i1_cond_reg, then_label, else_label);
            emit("%s:", then_label);
            generate_code(node->children[1]);
            if (!generator.last_instr_is_terminator)
                emit("  br label %%%s", merge_label);
            emit("%s:", else_label);
            generate_code(node->children[2]);
        }
        else
        { // IfStmt
            emit("  br i1 %s, label %%%s, label %%%s", i1_cond_reg, then_label, merge_label);
            emit("%s:", then_label);
            generate_code(node->children[1]);
        }
        if (!generator.last_instr_is_terminator)
            emit("  br label %%%s", merge_label);
        emit("%s:", merge_label);
        generator.last_instr_is_terminator = 0;
        free(i1_cond_reg);
        free(then_label);
        free(else_label);
        free(merge_label);
        return NULL;
    }
    if (strcmp(node->name, "WhileStmt") == 0)
    {
        char *cond_label = new_label("while_cond"), *body_label = new_label("while_body"), *after_label = new_label("while_after");
        emit("  br label %%%s", cond_label);
        emit("%s:", cond_label);
        char *cond_reg = generate_code(node->children[0]);
        char *i1_cond_reg = new_reg();
        emit("  %s = icmp ne i32 %s, 0", i1_cond_reg, cond_reg);
        free(cond_reg);
        emit("  br i1 %s, label %%%s, label %%%s", i1_cond_reg, body_label, after_label);
        emit("%s:", body_label);
        generate_code(node->children[1]);
        if (!generator.last_instr_is_terminator)
            emit("  br label %%%s", cond_label);
        emit("%s:", after_label);
        generator.last_instr_is_terminator = 0;
        free(i1_cond_reg);
        free(cond_label);
        free(body_label);
        free(after_label);
        return NULL;
    }

    // --- 5. 表达式 ---
    if (strcmp(node->name, "BinaryOp") == 0)
    {
        char *op_str = node->children[0]->name;
        char *left_reg = generate_code(node->children[1]);
        char *right_reg = generate_code(node->children[2]);
        char *res_reg = new_reg();
        char *type = "i32";
        char *op_code;
        int is_cmp = 0;
        if (strcmp(op_str, "+") == 0)
            op_code = "add";
        else if (strcmp(op_str, "-") == 0)
            op_code = "sub";
        else if (strcmp(op_str, "*") == 0)
            op_code = "mul";
        else if (strcmp(op_str, "/") == 0)
            op_code = "sdiv";
        else if (strcmp(op_str, "%") == 0)
            op_code = "srem";
        else
        {
            is_cmp = 1;
            if (strcmp(op_str, "==") == 0)
                op_code = "eq";
            else if (strcmp(op_str, "!=") == 0)
                op_code = "ne";
            else if (strcmp(op_str, "<") == 0)
                op_code = "slt";
            else if (strcmp(op_str, ">") == 0)
                op_code = "sgt";
            else if (strcmp(op_str, "<=") == 0)
                op_code = "sle";
            else if (strcmp(op_str, ">=") == 0)
                op_code = "sge";
            // 逻辑与、或在此简化，实际需要短路求值
            else if (strcmp(op_str, "&&") == 0)
                op_code = "and";
            else if (strcmp(op_str, "||") == 0)
                op_code = "or";
        }

        if (is_cmp)
        {
            emit("  %s = icmp %s %s %s, %s", res_reg, op_code, type, left_reg, right_reg);
        }
        else
        {
            emit("  %s = %s %s %s, %s", res_reg, op_code, type, left_reg, right_reg);
        }
        free(left_reg);
        free(right_reg);
        return res_reg;
    }
    if (strcmp(node->name, "UnaryOp") == 0)
    { /* ... 省略 ... */
    }
    if (strcmp(node->name, "FuncCall") == 0)
    {
        char *func_name = node->children[0]->name;
        Symbol *func_sym = lookup_symbol(func_name);
        if (!func_sym || !func_sym->is_func)
        { /* error */
            exit(1);
        }

        char *arg_reg_list[32];
        int arg_count = 0;
        if (node->children[1]->num_children > 0)
            process_actual_params(node->children[1]->children[0], arg_reg_list, &arg_count);

        char args_final_str[1024] = "";
        TypeNode *p = func_sym->params;
        for (int i = 0; i < arg_count; i++, p = p->next)
        {
            if (!p)
            { /* error */
                exit(1);
            }
            strcat(args_final_str, p->type);
            strcat(args_final_str, " ");
            strcat(args_final_str, arg_reg_list[i]);
            if (i < arg_count - 1)
                strcat(args_final_str, ", ");
            free(arg_reg_list[i]);
        }

        if (strcmp(func_sym->type, "void") == 0)
        {
            emit("  call void @%s(%s)", func_name, args_final_str);
            return NULL;
        }
        else
        {
            char *res_reg = new_reg();
            emit("  %s = call %s @%s(%s)", res_reg, func_sym->type, func_name, args_final_str);
            return res_reg;
        }
    }

    // --- 6. 表达式基础部分 ---
    if (strcmp(node->name, "ArrayAccess") == 0 || strcmp(node->name, "LVal") == 0 && node->num_children == 0)
    {
        // 当LVal被用作右值时，先获取其地址，再加载其值
        char *ptr_reg = generate_lval_address(node);
        char *val_reg = new_reg();
        // 需要确定加载的类型，这里简化为i32
        emit("  %s = load i32, i32* %s", val_reg, ptr_reg);
        free(ptr_reg);
        return val_reg;
    }
    if (node->num_children == 0)
    { // 叶子节点: INCONST, IDENT (未声明的)
        return strdup(node->name);
    }

    // 默认行为：处理单链式节点，如 Exp -> AddExp
    if (node->num_children == 1)
    {
        return generate_code(node->children[0]);
    }

    return NULL;
}

/* ================================================================== */
/* 新增/修改的辅助函数                                            */
/* ================================================================== */

// 递归处理语句块内的条目列表
static void process_block_item_list(Node *n)
{
    if (!n || n->num_children == 0)
        return;
    generate_code(n->children[0]);           // 处理当前 Decl 或 Stmt
    process_block_item_list(n->children[1]); // 递归处理列表剩余部分
}

// 递归处理变量定义列表
static void process_var_def_list(Node *n, const char *base_type)
{
    if (!n || n->num_children == 0)
        return;
    Node *var_def_node = n->children[0];
    char *var_name = var_def_node->children[0]->name;
    Node *dim_list = var_def_node->children[1];

    // 递归构建数组类型字符串
    char array_type_suffix[256] = "";
    int total_elements = 1;
    void build_array_type(Node * dim_node)
    {
        if (!dim_node || dim_node->num_children == 0)
            return;
        char *const_val_str = dim_node->children[0]->children[0]->children[0]->name;
        int dim_size = atoi(const_val_str);
        total_elements *= dim_size;
        char temp[64];
        sprintf(temp, "[%d x ", dim_size);
        strcat(array_type_suffix, temp);
        build_array_type(dim_node->children[1]);
    }
    build_array_type(dim_list);

    char final_type[512];
    strcpy(final_type, array_type_suffix);
    strcat(final_type, base_type);
    for (int i = 0; i < total_elements; i++)
    {
        if (strlen(array_type_suffix) > 0)
            strcat(final_type, "]");
    }

    // 发射alloca指令
    char *ptr_reg = new_reg();
    emit("  %s = alloca %s", ptr_reg, final_type);
    add_symbol(var_name, final_type, ptr_reg, 0);

    // 处理初始化... (此处简化)
    if (strcmp(var_def_node->name, "VarDef_Init") == 0)
    {
        // ... 初始化逻辑 ...
    }
    free(ptr_reg);

    if (n->num_children > 1)
        process_var_def_list(n->children[1], base_type);
}

// 递归为左值(LVal)生成地址，核心是 getelementptr
static char *generate_lval_address(Node *node)
{
    // 基本情况: LVal 是一个简单的 IDENT
    if (strcmp(node->name, "IDENT") == 0)
    {
        Symbol *sym = lookup_symbol(node->name);
        if (sym)
            return strdup(sym->llvm_reg);
        return NULL; // 错误：未定义的变量
    }

    // 递归情况: LVal 是一个数组访问 ArrayAccess
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        Node *base_node = node->children[0];
        Node *index_exp_node = node->children[1];

        // 1. 递归获取数组或子数组的基地址
        char *base_ptr_reg = generate_lval_address(base_node);
        // 2. 计算索引表达式的值
        char *index_val_reg = generate_code(index_exp_node);

        // 3. 获取基地址指针指向的类型 (例如 [10 x i32]*)
        Symbol *sym = lookup_symbol(base_node->name); // 简化：假设基地址总是变量名

        // 4. 发射 getelementptr 指令
        char *res_ptr_reg = new_reg();
        // GEP指令需要指针类型和元素类型
        // 例: %3 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i32 0, i32 %2
        // SysY的数组访问总是从指针开始，所以需要一个 i32 0 作为第一个索引
        emit("  %s = getelementptr inbounds %s, %s* %s, i32 0, i32 %s", res_ptr_reg, sym->type, sym->type, base_ptr_reg, index_val_reg);

        free(base_ptr_reg);
        free(index_val_reg);
        return res_ptr_reg;
    }
    return NULL;
}

// 其他递归辅助函数: process_param_types, process_formal_params, process_actual_params
// ... (与上一版相同，此处省略) ...