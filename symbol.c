#include "symbol.h"
#include <stdlib.h>
#include <string.h>



/* ------------------ 模块私有 ------------------ */
static Scope *current = NULL;
static char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    memcpy(p, s, n);
    return p;
}

/* ------------------ API 实现 ------------------ */
void enter_scope(void)
{
    Scope *s = calloc(1, sizeof(Scope));
    s->parent = current;
    current = s;
}

void exit_scope(void)
{
    Scope *p = current->parent;
    for (Symbol *sym = current->head, *n; sym; sym = n)
    {
        n = sym->next;
        free(sym->name);
        free(sym->type);
        free(sym->stack_ptr);
        free(sym);
    }
    free(current);
    current = p;
}

static Symbol *new_sym(const char *name, const char *type)
{
    Symbol *s = calloc(1, sizeof(Symbol));
    s->name = xstrdup(name);
    s->type = xstrdup(type);
    return s;
}

void add_symbol(const char *name, const char *type, const char *stack_ptr, bool is_const)
{
    Symbol *s = new_sym(name, type);
    s->stack_ptr = s->llvm_reg = xstrdup(stack_ptr);
    s->is_const = is_const;
    s->is_func = false;
    s->next = current->head;
    current->head = s;
}

void add_symbol_func(const char *name, const char *ret_type, TypeNode *params)
{
    Symbol *s = new_sym(name, ret_type);
    s->is_func = true;
    s->params = params;
    s->next = current->head;
    current->head = s;
}

Symbol *lookup_symbol(const char *name)
{
    for (Scope *sc = current; sc; sc = sc->parent)
        for (Symbol *sym = sc->head; sym; sym = sym->next)
            if (strcmp(sym->name, name) == 0)
                return sym;
    return NULL;
}
