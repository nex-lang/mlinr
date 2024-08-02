#ifndef GEN_H
#define GEN_H

#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    FILE* fp;
} Generator;

#define WO(fp, indent, format, ...) \
    do { \
        for (int i = 0; i < (indent); i++) fprintf((fp), "\t"); \
        fprintf((fp), (format), ##__VA_ARGS__); \
    } while (0)

Generator* gen_init(const char* filename);
void gen_free(Generator* gen);

void gen_stmt(AST_Instruction statement, Generator* gen);
void generate_program(AST_Node* node, Generator* gen);

void generate(AST_Node *root);

#endif // GEN_H
