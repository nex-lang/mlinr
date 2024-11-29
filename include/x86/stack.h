#ifndef X86_STACK_H
#define X86_STACK_H

#include <stdlib.h>
#include <inttypes.h>


typedef struct X86StackVar {
    uint64_t offset;
    int32_t id;
    size_t size;
} X86StackVar;

typedef struct X86Stack {
    uint64_t off;
    uint64_t size;
    uint64_t arg_size;

    size_t lsize;

    X86StackVar** vars;
    int32_t* regargs;

} X86Stack;

X86Stack* x86_stack();

void x86_push(X86Stack* stack, size_t offset, int32_t id, uint64_t size);
void x86_pop(X86Stack* stack);
void x86_spop(X86Stack* stack);
size_t x86_lookup_size(X86Stack* stack, int32_t id);
uint64_t x86_lookup_offset(X86Stack* stack, int32_t id);
int8_t x86_lookup_regarg(X86Stack* stack, int32_t id);
char* x86_get_regarg(uint8_t idx);

#endif // X86_STACK_H