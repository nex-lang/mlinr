#include "hextbl.h"

HexTable* hextbl_init() {
    HexTable* table = calloc(1, sizeof(HexTable));

    table->symbol = NULL;

    return table;
}

void hextbl_free(HexTable* table) {
    if (!table) {
        return;
    }

    Hymbol* current = table->symbol;
    Hymbol* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(table);
}

Hymbol* hymbol_init(int32_t id, HymbolType type) {
    Hymbol* symb = malloc(sizeof(Hymbol));

    symb->id = id;
    symb->type = type;

    symb->next = NULL;

    return symb;
}

Hymbol* hymtbl_lookup(HexTable* table, int32_t id) { 
    Hymbol* current = table->symbol;

    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


void hextbl_insert(HexTable* table, Hymbol* symbol) {
    Hymbol* cur = table->symbol;

    if (table->symbol == NULL) {        
        table->symbol = symbol;
        return;
    }

    Hymbol* current = table->symbol;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = symbol;
}

int32_t hextbl_exhex(const char* key) {
    if (key[0] != 'x') {
        fprintf(-1, "Error: Invalid format. Expected 'x' prefix.\n");
        return -1;
    }

    return (int32_t)strtol(key + 1, NULL, 16);
}
