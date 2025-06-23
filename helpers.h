#ifndef HELPERS_H
#define HELPERS_H
#include "node.h"
#include "symbol.h"
#include "value.h"
/* 简易判断：一个 InitVal 节点是否标量 */
int is_scalar_init(struct Node *n);
char *get_type_str(Node *type_node);
void count_params(Node *n, int *count);
int is_scalar_init(Node *n);
char *get_lval_type(Node *node);
#endif /* HELPERS_H */
