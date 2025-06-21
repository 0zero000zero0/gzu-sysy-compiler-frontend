#include "node.h"

Node* new_node(const char* name, int num_children, ...) {
  Node* node = (Node*)malloc(sizeof(Node));
  if (!node) {
    perror("FATAL: malloc failed for Node in new_node");
    exit(1);
  }

  if (name != NULL) {
    node->name = strdup(name);
    if (node->name == NULL) {
      perror("FATAL: strdup failed for node name in new_node");
      free(node);
      exit(1);
    }
  } else {
    node->name = NULL;
  }

  node->num_children = num_children;
  if (num_children > 0) {
    node->children = (Node**)malloc(num_children * sizeof(Node*));
    if (!node->children) {
      perror("FATAL: malloc failed for children array in new_node");
      if (node->name)
        free(node->name);
      free(node);
      exit(1);
    }
    va_list args;
    va_start(args, num_children);
    for (int i = 0; i < num_children; i++) {
      node->children[i] = va_arg(args, Node*);
    }
    va_end(args);
  } else {
    node->children = NULL;
  }
  return node;
}

// void print_AST(Node* node, int level) {
//   if (node == NULL) {
//     return;
//   }

//   if (node->name == NULL) {
//     return;
//   }

//   if (level > 0) {
//     for (int i = 0; i < level - 1; i++) {
//       printf("   ");
//     }
//     printf("|--");
//   }

//   if (node->num_children == 0) {
//     printf("%s\n", node->name ? node->name : "");
//   } else {
//     printf("<%s>\n", node->name ? node->name : "");
//   }

//   for (int i = 0; i < node->num_children; i++) {
//     print_AST(node->children[i], level + 1);
//   }
// }

void write_AST(Node* node, FILE* file, int level) {
  if (file == NULL) {
    fprintf(stderr, "FATAL: File pointer is NULL in write_AST\n");
    return;
  }
  if (node == NULL) {
    return;
  }

  if (node->name == NULL) {
    return;
  }

  if (level > 0) {
    for (int i = 0; i < level - 1; i++) {
      fprintf(file, "   ");
    }
    fprintf(file, "|--");
  }

  if (node->num_children == 0) {
    fprintf(file, "%s\n", node->name ? node->name : "");
  } else {
    fprintf(file, "<%s>\n", node->name ? node->name : "");
  }

  for (int i = 0; i < node->num_children; i++) {
    write_AST(node->children[i], file, level + 1);
  }
}