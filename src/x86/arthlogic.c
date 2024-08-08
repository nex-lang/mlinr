#include "x86/arthlogic.h"

#include "token.h"
#include "io.h"

#include <stdio.h>
#include <string.h>

char* get_movasm(X86Stack* stack, AST_Operand* op) {
    size_t size = 0;

    if (op->type == OPERAND_LITERAL) {
        return "mov";
    }

    if (op->type == OPERAND_VARIABLE) {
        size = x86_lookup_size(stack, op->value.variable);
        if (size == (size_t)-1) {
            return NULL;
        }
    }

    return (size == 64) ? "mov" : (size < 64) ? "movsx" : NULL;
}

char* get_refasm(X86Stack* stack, AST_Operand* operand, char** reg, uint8_t op) {
    size_t size = 0;
    char* kw = NULL;
    char* result = (char*)malloc(64);

    if (result == NULL) return NULL;

    if (operand->type == OPERAND_LITERAL) {
        switch (operand->value.literal.type) {
            case TOK_L_LINT:
            case TOK_L_LUINT:
            case TOK_L_DOUBLE:
                kw = "qword";
                size = 64;
                break;
            case TOK_L_INT:
            case TOK_L_UINT:
            case TOK_L_FLOAT:
            case TOK_L_SIZE:
                kw = "dword";
                size = 32;
                break;
            case TOK_L_SINT:
            case TOK_L_SUINT:
            case TOK_L_CHAR:
                kw = "word";
                size = 16;
                break;
            case TOK_L_SSINT:
            case TOK_L_SSUINT:
            case TOK_L_BOOL:
                kw = "byte";
                size = 8;
                break;
            default:
                free(result);
                return NULL; 
        }
    } else if (operand->type == OPERAND_VARIABLE) {
        size = x86_lookup_offset(stack, operand->value.variable);
        if (size == (size_t)-1) {
            free(result);
            return NULL;
        }
    }

    if (operand->type == OPERAND_VARIABLE) {
        snprintf(result, 64, "%s [rsp + %zu]", kw, size);
    } else {
        snprintf(result, 64, "%ld", operand->value.literal.type == TOK_L_LUINT ? operand->value.literal.value.uint : operand->value.literal.value.int_);
    }

    const char* reg_names[][4] = {
        {"rax", "eax", "ax", "al"},     // op == 0
        {"rcx", "ecx", "cx", "cl"},     // op == 1
        {"rdx", "edx", "dx", "dl"},     // op == 2
        {"rbx", "ebx", "bx", "bl"},     // op == 3
        {"r8",  "r8d", "r8w", "r8b"},   // op == 4
        {"r9",  "r9d", "r9w", "r9b"},   // op == 5
        {"r10", "r10d", "r10w", "r10b"}, // op == 6
        {"r11", "r11d", "r11w", "r11b"}  // op == 7
    };

    if (reg == NULL) {
        free(result);
        return NULL;
    }

    if (op < sizeof(reg_names)/sizeof(reg_names[0])) {
        switch (size) {
            case 64:
                snprintf(*reg, 16, "%s", reg_names[op][0]);
                break;
            case 32:
                snprintf(*reg, 16, "%s", reg_names[op][1]);
                break;
            case 16:
                snprintf(*reg, 16, "%s", reg_names[op][2]);
                break;
            case 8:
                snprintf(*reg, 16, "%s", reg_names[op][3]);
                break;
            default:
                free(result);
                free(*reg);
                return NULL; 
        }
    } else {
        free(result);
        free(*reg);
        return NULL;
    }

    return result;
}



void promote_register(FILE* fp, int op1, int op2, char** reg1, char** reg2, bool sign) {
    const char* registers[8][4] = {
        {"rax", "eax", "ax", "al"},     // op == 0
        {"rcx", "ecx", "cx", "cl"},     // op == 1
        {"rdx", "edx", "dx", "dl"},     // op == 2
        {"rbx", "ebx", "bx", "bl"},     // op == 3
        {"r8",  "r8d", "r8w", "r8b"},   // op == 4
        {"r9",  "r9d", "r9w", "r9b"},   // op == 5
        {"r10", "r10d", "r10w", "r10b"}, // op == 6
        {"r11", "r11d", "r11w", "r11b"}  // op == 7
    };

    int reg1_index = -1;
    int reg2_index = -1;

    for (int i = 0; i < 4; i++) {
        if (strcmp(*reg1, registers[op1][i]) == 0) {
            reg1_index = i;
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (strcmp(*reg2, registers[op2][i]) == 0) {
            reg2_index = i;
            break;
        }
    }

    printf("%d | %d\n", reg2_index, reg1_index);

    if (reg1_index != reg2_index) {
        if (reg1_index > reg2_index) {
            if (sign) {
                WO(fp, 1, "movsx %s, %s\n", registers[op1][reg1_index], registers[op2][reg2_index]);
            } else {
                WO(fp, 1, "movzx %s, %s\n", registers[op1][reg1_index], registers[op2][reg2_index]);
            }
            *reg2 = (char*)registers[op1][reg1_index];
        } else {
            if (sign) {
                WO(fp, 1, "movsx %s, %s\n", registers[op1][reg2_index], registers[op2][reg1_index]);
            } else {
                WO(fp, 1, "movzx %s, %s\n", registers[op1][reg2_index], registers[op2][reg1_index]);
            }
            *reg1 = (char*)registers[op1][reg2_index];
        }
    }
}


char* get_refasmid(X86Stack* stack, int32_t id) {
    size_t size = 0;
    char* result = (char*)malloc(64);

    size = x86_lookup_offset(stack, id);
    if (size == -1) {
        return NULL;
    }

    snprintf(result, 64, "[rsp + %zu]",  size);
    
    return result;
}


char* get_movasmid(X86Stack* stack, int32_t id) {
    size_t size = 0;

    size = x86_lookup_offset(stack, id);
    if (size == -1) {
        return NULL;
    }

    if (size == 64) {
        return "mov";
    } else if (size < 64) {
        return "movsx";
    }

    return NULL;
}