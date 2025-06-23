#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 内部小工具 */
static char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    memcpy(p, s, n);
    return p;
}

Value *new_value(const char *reg, const char *type)
{
    Value *v = malloc(sizeof(Value));
    v->reg = xstrdup(reg);
    v->type = xstrdup(type);
    return v;
}

void free_value(Value *v)
{
    if (!v)
        return;
    free(v->reg);
    free(v->type);
    free(v);
}

Value *make_float_constant(const char *literal)
{
    /* 1) 字符串 → float */
    float f = (float)atof(literal);

    /* 2) 获取 bit pattern */
    union {
        float f;
        uint32_t u;
    } bits = {.f = f};

    /* 3) 生成 bitcast 表达式 */
    char buf[64];
    snprintf(buf, sizeof(buf), "bitcast (i32 %u to float)", bits.u);

    return new_value(buf, "float");
}
