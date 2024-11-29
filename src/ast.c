#include "ast.h"

AST_Node* ast_init(NodeTypes type) {
    AST_Node* node = calloc(1, sizeof(AST_Node));

    node->type = type;
    
    return node;
}

void ast_free(AST_Node* node) {
    free(node);
}