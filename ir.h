#ifndef IR_H
#define IR_H
/**
 * @file ir.h
 * @brief 对外只暴露一个函数：把 AST → LLVM-IR(.ll)
 */
struct Node; /* 前置声明：语法树节点（由 parser 产出） */

void generate_llvm_ir(struct Node *ast, const char *output_path);

#endif /* IR_H */
