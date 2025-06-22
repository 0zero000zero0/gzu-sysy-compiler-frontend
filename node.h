#ifndef NODE_H
#define NODE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char *name;
    int num_children;
    struct Node **children;
} Node;



Node *new_node(const char *name, int num_children, ...);
void print_AST(Node *node, int level);
void write_AST(Node *node, FILE *file, int level);

#endif // NODE_H