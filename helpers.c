/***********************************************************************/
/* helper functions                                                    */
/***********************************************************************/

#include "helpers.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/**
 * @brief 判定 AST “初始化值”节点是否可被视为 **标量**。
 *
 * 标量定义：
 *  - 叶子节点（`num_children == 0`）
 *  - 名称为 `"ConstExp"`  —— 常量表达式
 *  - 名称为 `"Exp"`       —— 一般表达式（运行期求值）
 *
 * @param n  指向待判定的 AST 节点；允许为 `NULL`
 * @return   `1` 表示标量；`0` 表示聚合（数组 / 嵌套初值等）
 */
int is_scalar_init(struct Node *n)
{
    /* ① 空指针直接视作标量，调用方常用它来占位              */
    /* ② 叶子结点                                               */
    /* ③ 特定名字代表“表达式”节点                               */
    return (!n || n->num_children == 0) || strcmp(n->name, "ConstExp") == 0 || strcmp(n->name, "Exp") == 0;
}

/* ------------------------------------------------------------------ */
/**
 * @brief 将 SysY/BASIC 类型节点映射为 **LLVM IR** 类型字符串.
 *
 * > 目前仅支持 `int` / `float` / `void` 三种基本类型。
 *
 * @param type_node  语法树中描述类型的节点（`BType`、`FuncType` 等）
 * @return           常量字符串 `"i32"` / `"float"` / `"void"`
 */
char *get_type_str(Node *type_node)
{
    /* NOTE: 这里只比较名字，不考虑 const/unsigned 修饰等 */
    if (strcmp(type_node->name, "int") == 0)
        return "i32";
    if (strcmp(type_node->name, "float") == 0)
        return "float";
    return "void";
}

/* ------------------------------------------------------------------ */
/**
 * @brief 递归统计函数形参个数。
 *
 * 形参列表的文法形如：`FuncFParams -> FuncFParam (',' FuncFParam)*`
 * 递归地沿着右子链 (`n->children[1]`) 遍历即可。
 *
 * @param n      形参 AST 节点（`FuncFParams` / `FuncFParam`）
 * @param count  计数器指针，调用方应置 0；函数负责累加
 */
void count_params(Node *n, int *count)
{
    if (!n || n->num_children == 0)
        return; /* 空节点：结束递归 */

    (*count)++; /* 计入当前参数   */

    /* 若还有“剩余形参”，递归处理 */
    if (n->num_children > 1)
        count_params(n->children[1], count);
}

/* ------------------------------------------------------------------ */
/**
 * @brief 推断 **左值**（标识符 / 数组访问）的 LLVM Type 字符串。
 *
 * - `LVal(id)`             ➜ 变量本身的类型（符号表查询）
 * - `ArrayAccess(base, i)` ➜ 逐级剥离聚合维度，返回元素类型
 *
 * @param node  LVal 或 ArrayAccess 节点
 * @return      strdup() 出来的字符串（调用方负责 free）
 *
 * @warning 该函数会对符号表进行查找；若找不到符号直接返回 `NULL`。
 */
char *get_lval_type(Node *node)
{
    /* -------- 基本变量 ---------------------------------------- */
    if (strcmp(node->name, "LVal") == 0)
    {
        Symbol *sym = lookup_symbol(node->children[0]->name);
        return sym ? strdup(sym->type) : NULL;
    }

    /* -------- 多维数组访问：递归向内剥一维 --------------------- */
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        /*  先拿到基类型（可能仍是多维） */
        char *base_type = get_lval_type(node->children[0]);

        /*  尝试在形如 "[10 x [2 x i32]]" 中定位第一个 'x' */
        char *element_type = strchr(base_type, 'x');
        if (element_type)
        {
            /*  构造去掉最外层维度的字符串 */
            char buf[256];
            /* element_type + 2 跳过 "x " 两字符 */
            strcpy(buf, element_type + 2);
            /* 去掉结尾的 ']' */
            buf[strlen(buf) - 1] = '\0';

            free(base_type);
            return strdup(buf); /* 返回元素类型 */
        }

        /*  base_type 已经是最终元素类型（如 "i32" / "float"） */
        return base_type;
    }

    /* 非法节点 */
    return NULL;
}

/**
 * @brief 从 `BType | FuncType` 节点推断 **LLVM IR 基本类型字符串**。
 *
 * 仅支持 `int / float / void` 三种，在 SysY 语言中已足够。
 *
 * @param btype_node  类型节点（允许为 NULL）
 * @return            常量串 `"i32" / "float" / "void"`
 */
char *get_btype(Node *btype_node)
{
    if (btype_node && btype_node->num_children > 0)
    {
        Node *tok = btype_node->children[0];
        if (strcmp(tok->name, "int") == 0)
            return "i32";
        if (strcmp(tok->name, "float") == 0)
            return "float";
    }
    return "void";
}