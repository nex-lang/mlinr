#include "x86/arthlogic.h"

#include "token.h"

#include <stdio.h>

char* get_movasm(X86Stack* stack, AST_Operand* op) {
    size_t size = 0;
    char* instruction = (char*)malloc(6);

    if (op->type == OPERAND_VARIABLE) {
        size = x86_lookup_size(stack, op->value.variable);
        if (size == -1) {
            return NULL;
        }
    }

    if (size == 64 || op->value.literal.type == TOK_L_LINT || op->value.literal.type == TOK_L_LUINT || op->value.literal.type == TOK_L_DOUBLE) {
        instruction = "mov";
    } else if (size < 64) {
        if (op->value.literal.type == TOK_L_SSINT || op->value.literal.type == TOK_L_SINT || op->value.literal.type == TOK_L_INT) {
            instruction = "movsx";
        } else {
            instruction = "movzx";
        }
    }

    return instruction;
}


char* get_refasm(X86Stack* stack, AST_Operand* op) {
    size_t size = 0;
    char* kw = (char*)malloc(6);
    char* result = (char*)malloc(64);

    if (op->type == OPERAND_VARIABLE) {
        size = x86_lookup_size(stack, op->value.variable);
        if (size == -1) {
            free(result);
            return NULL;
        }
    } else if (op->type == OPERAND_LITERAL) {
        if (op->value.literal.type == TOK_L_LINT || op->value.literal.type == TOK_L_LUINT || op->value.literal.type == TOK_L_DOUBLE) {
            kw = "qword";
            size = 64;
        } else if (op->value.literal.type == TOK_L_INT || op->value.literal.type == TOK_L_UINT || op->value.literal.type == TOK_L_FLOAT || op->value.literal.type == TOK_L_SIZE) {
            kw = "dword";
            size = 32;
        } else if (op->value.literal.type == TOK_L_SINT || op->value.literal.type == TOK_L_SUINT || op->value.literal.type == TOK_L_CHAR) {
            kw = "word";
            size = 16;
        } else if (op->value.literal.type == TOK_L_SSINT || op->value.literal.type == TOK_L_SSUINT || op->value.literal.type == TOK_L_BOOL) {
            kw = "byte";
            size = 8;
        }
    }

    if (kw == NULL) {
        free(result);
        return NULL;
    }

    if (op->type == OPERAND_VARIABLE) {
        snprintf(result, 64, "%s [rsp + %zu]", kw, size);
        return result;
    }

    switch (op->value.literal.type) {
        case TOK_L_SSINT:
        case TOK_L_SINT:
        case TOK_L_INT:
        case TOK_L_LINT:
            snprintf(result, 64, "%s %ld", kw, op->value.literal.value.int_);
            break;
        case TOK_L_SSUINT:
        case TOK_L_SUINT:
        case TOK_L_UINT:
        case TOK_L_LUINT:
            snprintf(result, 64, "%s %ld", kw, op->value.literal.value.uint);
            break;
    }
    
    return result;
}