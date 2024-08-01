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

Symbol* symbol_init(int32_t id, SymbolType type, AST_Node* data) {
    Symbol* symb = malloc(sizeof(Symbol));

    symb->id = id;
    symb->type = type;
    symb->data = data;

    symb->next = NULL;

    return symb;
}

Symbol* symtbl_lookup(SymTable* table, int32_t id) {
    Symbol* current = table->symbol;

    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }

    return current;
}