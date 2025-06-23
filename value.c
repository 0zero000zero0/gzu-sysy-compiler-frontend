/**
 * @file value.c
 * @brief `Value` 结构体相关的创建/销毁与常量辅助函数
 */

#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/**
 * @brief 私有工具：等价于 `strdup`，失败时报错退出。
 *
 * @param s 源 C 字符串
 * @return  新分配并复制的字符串指针
 */
static char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1; /* 包含终止符 '\0' */
    char *p = malloc(n);
    memcpy(p, s, n); /* 拷贝包含 '\0' 的全部内容 */
    return p;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 构造一个新的 @c Value 结构体实例。
 *
 * @param reg   LLVM IR 寄存器 / 字面量 字符串（如 “%8” 或 “0”）
 * @param type  LLVM 类型字符串（例如 "i32", "float"）
 * @return      指向新分配的 Value*
 */
Value *new_value(const char *reg, const char *type)
{
    Value *v = malloc(sizeof(Value));
    v->reg = xstrdup(reg); /* 深拷贝，防止外部字符串被修改或释放 */
    v->type = xstrdup(type);
    return v;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 释放由 @c new_value / make_float_constant 创建的 @c Value。
 *
 * @param v 待销毁的 Value 指针（允许为 NULL）
 */
void free_value(Value *v)
{
    if (!v)
        return; /* 允许空指针，方便调用方 */
    free(v->reg);
    free(v->type);
    free(v);
}

/* ------------------------------------------------------------------ */
/**
 * @brief 将十进制浮点文本转成 LLVM “bitcast” 常量形式。
 *
 * 例如输入 `"1.732"` 会生成
 * @code
 *   bitcast (i32 1069259226 to float)
 * @endcode
 *
 * 这种形式可 100 % 还原 IEEE-754 位模式，避免
 * *lli / llc* 对十进制常量的精度再解析。
 *
 * @param literal  源浮点文本（如 "3.14", "6.02e23"）
 * @return         一个类型为 `"float"` 的 @c Value*
 *
 * @note 目前只支持 32 位单精度；如需双精度可自行扩展。
 */
Value *make_float_constant(const char *literal)
{
    /* ---------- 1) 文本 → float ----------------------------------- */
    float f = (float)atof(literal);

    /* ---------- 2) 取 IEEE-754 位模式 ----------------------------- */
    union {
        float f;
        uint32_t u;
    } bits = {.f = f};

    /* ---------- 3) 拼接 bitcast 字符串 ---------------------------- */
    char buf[64];
    snprintf(buf, sizeof(buf), "bitcast (i32 %u to float)", bits.u);

    /* 返回封装好的 Value*（reg = bitcast 表达式, type = "float"） */
    return new_value(buf, "float");
}
