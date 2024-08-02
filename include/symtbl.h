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
    unsigned int offset;
    
    struct Symbol* next;
} Symbol;

typedef struct SymTable {
    Symbol* symbol;
} SymTable;

SymTable* symtbl_init();
void symtbl_free(SymTable* table);

Symbol* symbol_init(char* id, SymbolType type, unsigned int scope, unsigned int offset);
Symbol* symtbl_lookup(SymTable* table, char* id, uint64_t scope);

int32_t symtbl_hash(const char* key, uint64_t scope);

#endif // SYMTBL_H
