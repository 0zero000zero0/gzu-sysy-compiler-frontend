/*======================================================================
 *  symbol.c ── 作用域 / 符号表 管理实现
 *====================================================================*/

#include "symbol.h"
#include <stdlib.h>
#include <string.h>

/* 当前（最内层）作用域指针 */
static Scope *current = NULL;

/* ------------------------------------------------------------------ */
/**
 * @brief 安全封装的 `strdup`，失败时报错退出。
 *
 * @param s  需要复制的以 '\0' 结尾的字符串
 * @return   指向新分配内存的拷贝
 */
static char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    memcpy(p, s, n);
    return p;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 进入一个新的词法/语义作用域。
 *
 * 在栈顶压入空 **Scope** 结点，并将 `current` 指针指向它。
 * 所有随后的符号都会挂到新的 scope 中。
 */
void enter_scope(void)
{
    Scope *s = calloc(1, sizeof(Scope)); /* head = NULL, parent=0 */
    s->parent = current;                 /* 链入旧作用域           */
    current = s;                         /* 更新栈顶               */
}

/* ------------------------------------------------------------------ */
/**
 * @brief 退出当前作用域并释放其所有符号。
 *
 * 遍历当前作用域的符号单链表，依次释放后，
 * 弹出作用域节点本身，`current` 指回上一层。
 */
void exit_scope(void)
{
    /* --- 1. 释放本作用域下的所有 Symbol --------------------- */
    Scope *parent = current->parent;

    for (Symbol *sym = current->head, *next; sym; sym = next)
    {
        next = sym->next; /* 提前保存下一个节点指针 */
        free(sym->name);
        free(sym->type);
        free(sym->reg_ptr);
        free(sym);
    }

    /* --- 2. 弹出作用域结点 ---------------------------------- */
    free(current);
    current = parent;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 创建一个新的 Symbol 结构体（仅初始化名字/类型）。
 *
 * @param name  标识符名称
 * @param type  LLVM 类型字符串（或函数返回类型）
 * @return      已分配并填充好的 Symbol*
 *
 * @note 其余字段由更高层函数再补齐。
 */
static Symbol *new_sym(const char *name, const char *type)
{
    Symbol *s = calloc(1, sizeof(Symbol));
    s->name = xstrdup(name);
    s->type = xstrdup(type);
    return s;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 向当前作用域添加一个 **变量** 符号。
 *
 * @param name       变量名
 * @param type       LLVM 类型字符串
 * @param reg_ptr  该变量 `alloca` 后的栈指针寄存器（如 "%5"）
 * @param is_const   是否为常量（SysY `const` 修饰）
 */
void add_symbol(const char *name, const char *type, const char *reg_ptr, bool is_const)
{
    Symbol *s = new_sym(name, type);
    s->reg_ptr = xstrdup(reg_ptr); /* 存储栈地址指针             */
    s->is_const = is_const;
    s->is_func = false;

    /* 头插到当前作用域的符号链表 */
    s->next = current->head;
    current->head = s;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 向当前作用域添加一个 **函数** 符号。
 *
 * @param name      函数名
 * @param ret_type  返回值 LLVM 类型
 * @param params    形参类型链表（可为 NULL）
 */
void add_symbol_func(const char *name, const char *ret_type, TypeNode *params)
{
    Symbol *s = new_sym(name, ret_type);
    s->is_func = true;
    s->params = params;

    /* 头插到符号链表 */
    s->next = current->head;
    current->head = s;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 在作用域链中查找指定名称的符号。
 *
 * 从当前作用域向外逐层搜索，返回第一处命中的符号指针。
 *
 * @param name  要查找的标识符
 * @return      `Symbol*` 若找到；否则 `NULL`
 */
Symbol *lookup_symbol(const char *name)
{
    /* 从内到外遍历作用域 */
    for (Scope *sc = current; sc; sc = sc->parent)
        for (Symbol *sym = sc->head; sym; sym = sym->next)
            if (strcmp(sym->name, name) == 0)
                return sym;

    return NULL; /* 未找到 */
}
