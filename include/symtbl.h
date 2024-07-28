#ifndef SYMTBL_H
#define SYMTBL_H

#include "ast.h"

#include <inttypes.h>
#include <stdlib.h>

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_ERR
} SymbolType;

typedef struct Symbol {
    int32_t id;
    SymbolType type;
    AST_Node* data;
    struct Symbol* next;
} Symbol;

typedef struct SymTable {
    Symbol* symbol;
} SymTable;

SymTable* symtbl_init();
void symtbl_free(SymTable* table);

Symbol* symbol_init(int32_t id, SymbolType type, AST_Node* data);
Symbol* symtbl_lookup(SymTable* table, int32_t id);

#endif // SYMTBL_H
