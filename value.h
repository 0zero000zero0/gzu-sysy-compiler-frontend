#ifndef VALUE_H
#define VALUE_H
#include <stdint.h>

typedef struct Value
{
    char *reg;  /* "%12" / "0" / "bitcast (...)" 等 */
    char *type; /* "i32" / "float" / "i1" …         */
} Value;

Value *new_value(const char *reg, const char *type);
void free_value(Value *v);

Value *make_float_constant(const char *decimal_literal);

#endif /* VALUE_H */
