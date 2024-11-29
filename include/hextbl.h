#ifndef HEXTBL_H
#define HEXTBL_H

#include <inttypes.h>
#include <stdlib.h>

typedef enum {
    HYMBOL_STRUCT,
    HYMBOL_FUNCTION,
    HYMBOL_MEP
} HymbolType;

typedef struct Hymbol {
    int32_t id;
    
    HymbolType type;
    unsigned int offset;
    
    struct Symbol* next;
} Hymbol;

typedef struct HexTable {
    Hymbol* symbol;
} HexTable;

HexTable* hextbl_init();
void hextbl_free(HexTable* table);

Hymbol* hymbol_init(int32_t id, HymbolType type);
Hymbol* hymtbl_lookup(HexTable* table, int32_t id);

void hextbl_insert(HexTable* table, Hymbol* symbol);
int32_t hextbl_exhex(const char* key);

#endif // HEXTBL_H
