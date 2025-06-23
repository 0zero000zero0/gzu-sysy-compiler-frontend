#ifndef VALUE_H
#define VALUE_H
#include <stdint.h>

typedef struct Value
{
    char *reg;  /* "%12" / "0" / "bitcast (...)" 等 */
    char *type; /* "i32" / "float" / "i1" …         */
} Value;

/* ------------  API  ------------- */
Value *new_value(const char *reg, const char *type);
void free_value(Value *v);

/* 十进制浮点 → LLVM 可接受格式（bitcast i32→float） */
Value *make_float_constant(const char *decimal_literal);

#endif /* VALUE_H */
