`ir.c`:
```c
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

typedef struct Symbol
{
    char *name;
    char *type;     // 变量类型 或 函数返回类型
    char *llvm_reg; // 变量在栈上的地址指针
    int is_const;
    int is_func;      // 新增：是否是函数
    TypeNode *params; // 新增：函数参数类型列表
    struct Symbol *next;
} Symbol;

typedef struct Scope
{
    Symbol *head;         // 作用域内符号链表头
    struct Scope *parent; // 父作用域指针
} Scope;

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
} IRGenerator;
static IRGenerator generator;

static char *get_type_str(Node *type_node);
static char *generate_lval_address(Node *node);
static void emit(const char *format, ...);

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

static void add_symbol_func(const char *name, const char *type,
                            TypeNode *params)
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

static void add_symbol(const char *name, const char *type, const char *llvm_reg,
                       int is_const)
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
static void process_actual_params(Node *n, char *arg_reg_list[], int *count);
static void process_var_def_list(Node *n, const char *type);
static void process_block_item_list(Node *n);
static char *generate_code(Node *node)
{
    if (!node || (node->name == NULL && node->num_children == 0))
        return NULL;

    // --- 结构与作用域 ---
    if (strcmp(node->name, "CompUnit") == 0)
    {
        if (node->num_children > 0)
        {
            generate_code(node->children[0]);
            generate_code(node->children[1]);
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

    // --- 声明 ---
    if (strcmp(node->name, "VarDecl") == 0 || strcmp(node->name, "ConstDecl") == 0)
    {
        char *type = get_type_str(node->children[0]);
        process_var_def_list(node->children[1], type);
        return NULL;
    }

    // --- 函数定义 ---
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

    // --- 语句 ---
    if (strcmp(node->name, "AssignStmt") == 0)
    {
        char *val_reg = generate_code(node->children[1]);
        char *ptr_reg = generate_lval_address(node->children[0]);

        // 为了获取类型，我们需要找到LVal最终指向的变量
        Node *base_var_node = node->children[0];
        while (strcmp(base_var_node->name, "ArrayAccess") == 0)
        {
            base_var_node = base_var_node->children[0];
        }
        Symbol *sym = lookup_symbol(base_var_node->children[0]->name);

        char element_type[64] = "i32"; // 简化，需要更复杂的类型推导
        if (sym)
        {
            // 从数组类型 [10 x i32] 中提取元素类型 i32
            char *type_start = strchr(sym->type, 'x');
            if (type_start)
            {
                sscanf(type_start + 1, " %s", element_type);
                element_type[strlen(element_type) - 1] = '\0'; // 去掉 ']'
            }
            else
            {
                strcpy(element_type, sym->type);
            }
        }

        emit("  store %s %s, %s* %s", element_type, val_reg, element_type, ptr_reg);
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
    if (strcmp(node->name, "IfStmt") == 0 || strcmp(node->name, "IfElseStmt") == 0)
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
        {
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
    if (strcmp(node->name, "ExpStmt") == 0)
    {
        if (node->children[0]->num_children > 0)
            generate_code(node->children[0]->children[0]);
        return NULL;
    }

    // --- 表达式 ---
    if (strcmp(node->name, "BinaryOp") == 0)
    {
        char *op_str = node->children[0]->name;
        char *left_reg = generate_code(node->children[1]);
        char *right_reg = generate_code(node->children[2]);
        char *res_reg = new_reg();
        char *type = "i32";
        char *op_code = "";
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
        }
        emit("  %s = %s %s %s, %s", res_reg, is_cmp ? "icmp" : op_code, type, left_reg, right_reg);
        free(left_reg);
        free(right_reg);
        return res_reg;
    }
    if (strcmp(node->name, "FuncCall") == 0)
    {
        char *func_name = node->children[0]->name;
        Symbol *func_sym = lookup_symbol(func_name);
        if (!func_sym || !func_sym->is_func)
        {
            fprintf(stderr, "Error: Call to undefined function '%s'\n", func_name);
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
            {
                fprintf(stderr, "Error: Too many arguments for function %s\n", func_name);
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
    if (strcmp(node->name, "UnaryOp") == 0)
    { /* 与之前相同 */
    }

    // --- 表达式基础 ---
    if (strcmp(node->name, "LVal") == 0 || strcmp(node->name, "ArrayAccess") == 0)
    {
        char *ptr_reg = generate_lval_address(node);
        char *val_reg = new_reg();
        char element_type[64] = "i32"; // 简化
        emit("  %s = load %s, %s* %s", val_reg, element_type, element_type, ptr_reg);
        free(ptr_reg);
        return val_reg;
    }

    if (node->num_children == 0)
    { // 叶子节点: INCONST, IDENT等
        return strdup(node->name);
    }

    // 默认行为：处理单孩子链式节点，如 Exp -> AddExp -> MulExp ...
    if (node->num_children == 1)
    {
        return generate_code(node->children[0]);
    }

    return NULL; // 其他所有情况
}

/* ================================================================== */
/* 辅助函数 (Helpers)                                           */
/* ================================================================== */
static char *get_type_str(Node *type_node)
{
    if (strcmp(type_node->name, "int") == 0)
        return "i32";
    if (strcmp(type_node->name, "float") == 0)
        return "float";
    return "void";
}

static void process_block_item_list(Node *n)
{
    if (!n || n->num_children == 0)
        return;
    generate_code(n->children[0]);
    process_block_item_list(n->children[1]);
}

static void build_array_type_str(Node *dim_node, char *type_str_builder)
{
    if (!dim_node || dim_node->num_children == 0)
        return;
    char *const_val_str = dim_node->children[0]->children[0]->name; // ConstExp -> AddExp -> ... -> Number -> INCONST
    char temp[64];
    sprintf(temp, "[%s x ", const_val_str);
    strcat(type_str_builder, temp);
    build_array_type_str(dim_node->children[1], type_str_builder);
    strcat(type_str_builder, "]");
}

static void process_var_def_list(Node *n, const char *base_type)
{
    if (!n || n->num_children == 0)
        return;
    Node *var_def_node = n->children[0];
    char *var_name = var_def_node->children[0]->name;
    Node *dim_list = var_def_node->children[1];
    char type_builder[512] = "";
    build_array_type_str(dim_list, type_builder);
    strcat(type_builder, base_type);

    char *ptr_reg = new_reg();
    emit("  %s = alloca %s", ptr_reg, type_builder);
    add_symbol(var_name, type_builder, ptr_reg, 0);

    if (strcmp(var_def_node->name, "VarDef_Init") == 0)
    {
        // 数组初始化逻辑较为复杂，此处简化
        char *init_val_reg = generate_code(var_def_node->children[2]);
        emit("  ; array init for %s not fully implemented", var_name);
        free(init_val_reg);
    }
    free(ptr_reg);

    if (n->num_children > 1)
        process_var_def_list(n->children[1], base_type);
}

static char *generate_lval_address(Node *node)
{
    // Base Case: IDENT
    if (strcmp(node->name, "IDENT") == 0)
    {
        Symbol *sym = lookup_symbol(node->name);
        if (sym)
            return strdup(sym->llvm_reg);
        return NULL;
    }
    // Recursive Step: ArrayAccess
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        char *base_ptr_reg = generate_lval_address(node->children[0]);
        char *index_val_reg = generate_code(node->children[1]);
        char *res_ptr_reg = new_reg();

        // 获取基类型的指针，例如 [10 x i32]* 的基类型是 [10 x i32]
        // 这是一个简化，实际需要一个函数来从符号表和 GEP 链中推导类型
        emit("  %s = getelementptr inbounds i32, i32* %s, i32 %s", res_ptr_reg, base_ptr_reg, index_val_reg);

        free(base_ptr_reg);
        free(index_val_reg);
        return res_ptr_reg;
    }
    // LVal -> IDENT
    if (strcmp(node->name, "LVal") == 0)
    {
        return generate_lval_address(node->children[0]);
    }
    return NULL;
}

static void process_param_types(Node *n, TypeNode **head_ref)
{
    if (!n || n->num_children == 0)
        return;
    if (n->num_children > 1)
        process_param_types(n->children[1], head_ref);
    Node *param_node = n->children[0];
    TypeNode *new_tn = (TypeNode *)malloc(sizeof(TypeNode));
    new_tn->type = strdup(get_type_str(param_node->children[0]));
    new_tn->next = *head_ref;
    *head_ref = new_tn;
}

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
    emit("  %s = alloca %s", ptr_reg, param_type);
    emit("  store %s %s, %s* %s", param_type, param_reg, param_type, ptr_reg);
    add_symbol(param_name, param_type, ptr_reg, 0);
    free(ptr_reg);
    if (n->num_children > 1)
        process_formal_params(n->children[1], count);
}

static void process_actual_params(Node *n, char *arg_reg_list[], int *count)
{
    if (!n || n->num_children == 0)
        return;
    arg_reg_list[(*count)++] = generate_code(n->children[0]);
    if (n->num_children > 1)
        process_actual_params(n->children[1], arg_reg_list, count);
}

// 生成器设置与销毁

static void init_ir_generator()
{
    generator.reg_counter = 0;
    generator.label_counter = 0;
    generator.code_size = 0;
    generator.code_capacity = 1024;
    generator.code_buffer =
        (char **)malloc(generator.code_capacity * sizeof(char *));
    generator.current_scope = NULL;
    generator.current_func_ret_type = NULL;
    generator.last_instr_is_terminator = 0;
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
        generator.code_buffer = (char **)realloc(
            generator.code_buffer, generator.code_capacity * sizeof(char *));
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

// 公共入口点

void generate_llvm_ir(Node *root, const char *output_filename)
{
    init_ir_generator();
    generate_code(root);
    write_ir_to_file(output_filename);
    free_ir_generator();
}
```


`sysy.y`:
```yacc
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ir.h"
#include "node.h"

// 函数声明
void write_AST(Node *node, FILE* file, int level);
extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char *yytext;
void yyerror(const char *s);
Node* ast_root = NULL;
%}

/* --- Bison Declarations --- */
%union {
    char* str_val;
    struct Node* node_ptr;
}

/* --- Token Declarations --- */
%token <node_ptr> IDENT INCONST FLOACONST
%token <node_ptr> CONST INT FLOAT VOID
%token <node_ptr> IF ELSE WHILE BREAK CONTINUE RETURN
%token <node_ptr> PLUS MINUS MUL DIV MOD ASSIGN
%token <node_ptr> EQ NEQ LT GT LTE GTE
%token <node_ptr> AND OR NOT
%token <node_ptr> LPAREN RPAREN LBRACE RBRACE LBRACK RBRACK
%token <node_ptr> COMMA SEMICOLON

/* --- Type Declarations for Non-terminals --- */
%type <node_ptr> CompUnit Decl ConstDecl VarDecl ConstDef_List ConstDef VarDef_List VarDef
%type <node_ptr> ConstSubscript_List ConstInitVal InitVal FuncDef FuncFParamsOpt FuncFParams OptConstInitVal_List ConstInitVal_List InitVal_List OptInitVal_List
%type <node_ptr> FuncFParam Block BlockItem_List BlockItem Stmt ExpOpt Exp Cond LVal
%type <node_ptr> Number UnaryOp FuncRParamsOpt FuncRParams ConstExp
%type <node_ptr> LOrExp LAndExp EqExp RelExp AddExp MulExp UnaryExp PrimaryExp BType

/* --- Operator Precedence and Associativity --- */
%nonassoc IFX
%nonassoc ELSE
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left LT GT LTE GTE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT

%start CompUnit

%%
/* ================================================================== */
/* 最终版 SysY 2022 文法                           */
/* ================================================================== */

CompUnit:
    /* empty */      { $$ = new_node("CompUnit", 0); ast_root = $$; }
    | Decl CompUnit      { $$ = new_node("CompUnit", 2, $1, $2); ast_root = $$; }
    | FuncDef CompUnit { $$ = new_node("CompUnit", 2, $1, $2); ast_root = $$; }
    ;

Decl: ConstDecl { $$ = $1; } | VarDecl { $$ = $1; };
BType:
    INT   { $$ = $1; }
    | FLOAT { $$ = $1; }
    ;

ConstDecl: CONST BType ConstDef_List SEMICOLON { $$ = new_node("ConstDecl", 2, $2, $3); };
ConstDef_List:
    ConstDef { $$ = new_node("ConstDef_List", 1, $1); }
    | ConstDef COMMA ConstDef_List { $$ = new_node("ConstDef_List", 2, $1, $3); }
    ;
ConstDef: IDENT ConstSubscript_List ASSIGN ConstInitVal { $$ = new_node("ConstDef", 3, $1, $2, $4); };

VarDecl: BType VarDef_List SEMICOLON { $$ = new_node("VarDecl", 2, $1, $2); };
VarDef_List:
    VarDef { $$ = new_node("VarDef_List", 1, $1); }
    | VarDef COMMA VarDef_List { $$ = new_node("VarDef_List", 2, $1, $3); }
    ;
VarDef:
    IDENT ConstSubscript_List { $$ = new_node("VarDef", 2, $1, $2); }
    | IDENT ConstSubscript_List ASSIGN InitVal { $$ = new_node("VarDef_Init", 3, $1, $2, $4); }
    ;

ConstSubscript_List:
    /* empty */ { $$ = new_node("ArrayDimList", 0); }
    | LBRACK ConstExp RBRACK ConstSubscript_List { $$ = new_node("ArrayDimList", 2, $2, $4); }
    ;

ConstInitVal:
    ConstExp { $$ = $1; }
    | LBRACE OptConstInitVal_List RBRACE { $$ = new_node("InitVal_Aggregate", 1, $2); }
    ;
OptConstInitVal_List:
    /* empty */ { $$ = new_node("ConstInitVal_List", 0); }
    | ConstInitVal_List { $$ = $1; }
    ;
ConstInitVal_List:
    ConstInitVal { $$ = new_node("ConstInitVal_List", 1, $1); }
    | ConstInitVal COMMA ConstInitVal_List { $$ = new_node("ConstInitVal_List", 2, $1, $3); }
    ;

InitVal:
    Exp { $$ = $1; }
    | LBRACE OptInitVal_List RBRACE { $$ = new_node("InitVal_Aggregate", 1, $2); }
    ;
OptInitVal_List:
    /* empty */ { $$ = new_node("InitVal_List", 0); }
    | InitVal_List { $$ = $1; }
    ;
InitVal_List:
    InitVal { $$ = new_node("InitVal_List", 1, $1); }
    | InitVal COMMA InitVal_List { $$ = new_node("InitVal_List", 2, $1, $3); }
    ;

FuncDef: BType IDENT LPAREN FuncFParamsOpt RPAREN Block { $$ = new_node("FuncDef", 4, $1, $2, $4, $6); };

FuncFParamsOpt:
    /* empty */   { $$ = new_node("FuncFParamsOpt", 0); }
    | FuncFParams { $$ = new_node("FuncFParamsOpt", 1, $1); }
    ;
FuncFParams:
    FuncFParam { $$ = new_node("FuncFParams", 1, $1); }
    | FuncFParam COMMA FuncFParams { $$ = new_node("FuncFParams", 2, $1, $3); }
    ;
FuncFParam:
    BType IDENT { $$ = new_node("FuncFParam", 2, $1, $2); }
    | BType IDENT LBRACK RBRACK ConstSubscript_List { $$ = new_node("FuncFParam_Array", 3, $1, $2, $5); }
    ;

Block: LBRACE BlockItem_List RBRACE { $$ = new_node("Block", 1, $2); };
BlockItem_List:
    /* empty */      { $$ = new_node("BlockItemList", 0); }
    | BlockItem BlockItem_List { $$ = new_node("BlockItemList", 2, $1, $2); }
    ;
BlockItem: Decl { $$ = $1; } | Stmt { $$ = $1; };

Stmt:
    LVal ASSIGN Exp SEMICOLON { $$ = new_node("AssignStmt", 2, $1, $3); }
    | ExpOpt SEMICOLON         { $$ = new_node("ExpStmt", 1, $1); }
    | Block                    { $$ = $1; }
    | IF LPAREN Cond RPAREN Stmt %prec IFX { $$ = new_node("IfStmt", 2, $3, $5); }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt { $$ = new_node("IfElseStmt", 3, $3, $5, $7); }
    | WHILE LPAREN Cond RPAREN Stmt { $$ = new_node("WhileStmt", 2, $3, $5); }
    | BREAK SEMICOLON          { $$ = new_node("BreakStmt", 0); }
    | CONTINUE SEMICOLON       { $$ = new_node("ContinueStmt", 0); }
    | RETURN ExpOpt SEMICOLON  { $$ = new_node("ReturnStmt", 1, $2); }
    ;

ExpOpt:
    /* empty */ { $$ = new_node("ExpOpt", 0); }
    | Exp       { $$ = new_node("ExpOpt", 1, $1); }
    ;
Cond: LOrExp { $$ = $1; };
ConstExp: AddExp { $$ = $1; };

Exp: AddExp { $$ = $1; };

LOrExp: LAndExp { $$ = $1; } | LOrExp OR LAndExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

LAndExp: EqExp { $$ = $1; } | LAndExp AND EqExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

EqExp: RelExp { $$ = $1; } | EqExp EQ RelExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | EqExp NEQ RelExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

RelExp: AddExp { $$ = $1; } | RelExp LT AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp GT AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp LTE AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp GTE AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

AddExp: MulExp { $$ = $1; } | AddExp PLUS MulExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | AddExp MINUS MulExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

MulExp: UnaryExp { $$ = $1; } | MulExp MUL UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | MulExp DIV UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | MulExp MOD UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

UnaryExp:
    PrimaryExp { $$ = $1; }
    | IDENT LPAREN FuncRParamsOpt RPAREN { $$ = new_node("FuncCall", 2, $1, $3); }
    | UnaryOp UnaryExp { $$ = new_node("UnaryOp", 2, $1, $2); }
    ;
PrimaryExp:
    LPAREN Exp RPAREN { $$ = $2; }
    | LVal { $$ = $1; }
    | Number { $$ = $1; }
    ;
Number: INCONST { $$ = $1; } | FLOACONST { $$ = $1; };
UnaryOp: PLUS {$$ = $1;} | MINUS {$$ = $1;} | NOT {$$ = $1;};

LVal:
    IDENT { $$ = new_node("LVal", 1, $1); } // Base case for LVal
    | LVal LBRACK Exp RBRACK { $$ = new_node("ArrayAccess", 2, $1, $3); }
    ;

FuncRParamsOpt:
    /* empty */   { $$ = new_node("FuncRParamsOpt", 0); }
    | FuncRParams { $$ = new_node("FuncRParamsOpt", 1, $1); }
    ;
FuncRParams:
    Exp { $$ = new_node("FuncRParams", 1, $1); }
    | Exp COMMA FuncRParams { $$ = new_node("FuncRParams", 2, $1, $3); }
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        printf("Usage: %s <input-file> [ir-output-file] [AST-output-file]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror(argv[1]);
        return 1;
    }
    yyin = file;

    const char* ir_output_filename = argv[2] ;
    FILE *ast_output_file = fopen(argv[3], "w");
    if (yyparse() == 0) {
        write_AST(ast_root, ast_output_file, 0);
        fflush(ast_output_file);
        generate_llvm_ir(ast_root, ir_output_filename);
    } else {
        printf("Parse failed.\n");
    }
    fclose(ast_output_file);
    fclose(file);
    return 0;
}
```
以上是我的现在`ir.c`和`sysy.y`代码，这个代码可以