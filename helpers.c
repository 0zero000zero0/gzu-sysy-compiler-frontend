#include "helpers.h"
#include "value.h"

#include <string.h>
int is_scalar_init(struct Node *n)
{
    /* 叶子节点 / ConstExp / Exp 视为标量 */
    return (!n || n->num_children == 0) || strcmp(n->name, "ConstExp") == 0 || strcmp(n->name, "Exp") == 0;
}

/* ================================================================== */
/* 辅助函数 (Helpers)                                           */
/* ================================================================== */
// get_type_str: 从类型节点获取LLVM类型字符串
char *get_type_str(Node *type_node)
{
    if (strcmp(type_node->name, "int") == 0)
        return "i32";
    if (strcmp(type_node->name, "float") == 0)
        return "float";
    return "void";
}

void count_params(Node *n, int *count)
{
    if (!n || n->num_children == 0)
        return;
    (*count)++;
    if (n->num_children > 1)
    {
        count_params(n->children[1], count);
    }
}



// generate_lval_address: 为左值(LVal)生成地址
char *get_lval_type(Node *node)
{
    if (strcmp(node->name, "LVal") == 0)
    {
        Symbol *sym = lookup_symbol(node->children[0]->name);
        return strdup(sym->type);
    }
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        char *base_type = get_lval_type(node->children[0]);
        // 从聚合类型 [N x T] 中推断出元素类型 T
        char *element_type = strchr(base_type, 'x');
        if (element_type)
        {
            char result[256];
            // element_type+2 跳过 "x "
            strcpy(result, element_type + 2);
            // 去掉末尾的 ']'
            result[strlen(result) - 1] = '\0';
            free(base_type);
            return strdup(result);
        }
        return base_type;
    }
    return NULL;
}