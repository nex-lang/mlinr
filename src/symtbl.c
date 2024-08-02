#include "symtbl.h"

SymTable* symtbl_init() {
    SymTable* table = calloc(1, sizeof(SymTable));

    table->symbol = NULL;

    return table;
}

void symtbl_free(SymTable* table) {
    if (!table) {
        return;
    }

    Symbol* current = table->symbol;
    Symbol* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(table);
}

Symbol* symbol_init(char* id, SymbolType type, unsigned int scope, unsigned int offset) {
    Symbol* symb = malloc(sizeof(Symbol));

    symb->id = symtbl_hash(id, scope);
    symb->type = type;

    symb->next = NULL;

    return symb;
}

Symbol* symtbl_lookup(SymTable* table, char* id, uint64_t scope) { 
    uint32_t hash_id = symtbl_hash((const char*)id, scope);
    Symbol* current = table->symbol;

    while (current != NULL) {
        if (current->id == hash_id) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


int32_t symtbl_hash(const char* key, uint64_t scope) {
    int32_t hash_val = 5381;

    while (*key) {
        hash_val = ((hash_val << 5) + hash_val) + *key++;
    }

    hash_val = ((hash_val << 5) + hash_val) + scope;

    return hash_val;
}