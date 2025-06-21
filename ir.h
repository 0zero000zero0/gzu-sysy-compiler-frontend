#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "node.h"
#include <stdio.h>

void generate_llvm_ir(Node *root, const char *output_filename);


#endif // IR_GENERATOR_H