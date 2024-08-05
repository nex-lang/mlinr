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

    size_t size;
    X86StackVar** vars;
} X86Stack;

X86Stack* x86_stack();

void x86_push(X86Stack* stack, size_t offset, int32_t id, uint64_t size);
void x86_pop(X86Stack* stack);
void x86_spop(X86Stack* stack);
size_t x86_lookup_size(X86Stack* stack, int32_t id);

#endif // X86_STACK_H