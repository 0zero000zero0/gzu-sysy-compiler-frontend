#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdbool.h>

typedef struct TypeNode
{
    char *type;            /* LLVM 类型字符串         */
    struct TypeNode *next; /* 指向下一个形参类型节点。 */
} TypeNode;

typedef struct Symbol
{
    char *name;
    char *type;
    char *reg_ptr; // 变量在函数栈帧里的 alloca 指针名称（如 %3）。
    bool is_const, is_func;
    TypeNode *params;    //	函数形参类型链表。仅当 is_func == true 有意义
    struct Symbol *next; // 链到同一作用域里的下一个符号
} Symbol;

typedef struct Scope
{
    Symbol *head;
    struct Scope *parent; // 指向外层作用域；符号查找时自内向外逐层遍历。
} Scope;

typedef struct LoopLabels
{
    char *continue_label;    // continue 跳转的目标
    char *break_label;       // break 跳转的目标
    struct LoopLabels *next; // 指向外层循环的标签 (形成链表/栈)
} LoopLabels;

/* 不对外暴露 Symbol/Scope 细节 —— 仅用不透明指针 */
void enter_scope(void);
void exit_scope(void);

/* 变量 / 函数 */
void add_symbol(const char *name, const char *llvm_type, const char *reg_ptr, bool is_const);
void add_symbol_func(const char *name, const char *ret_type, TypeNode *params);

Symbol *lookup_symbol(const char *name);

#endif /* SYMBOL_H */
