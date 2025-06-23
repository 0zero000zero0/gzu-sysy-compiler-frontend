#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdbool.h>

typedef struct TypeNode
{               /* 只是链表，放这里方便共享 */
    char *type; /* LLVM 类型字符串         */
    struct TypeNode *next;
} TypeNode;

typedef struct Symbol
{
    char *name, *type, *stack_ptr, *llvm_reg;
    bool is_const, is_func;
    TypeNode *params;
    struct Symbol *next;
} Symbol;

typedef struct Scope
{
    Symbol *head;
    struct Scope *parent;
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
void add_symbol(const char *name, const char *llvm_type, const char *stack_ptr, bool is_const);
void add_symbol_func(const char *name, const char *ret_type, TypeNode *params);

Symbol *lookup_symbol(const char *name);

#endif /* SYMBOL_H */
