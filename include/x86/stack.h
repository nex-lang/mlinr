#ifndef X86_STACK_H
#define X86_STACK_H

#include <stdlib.h>
#include <inttypes.h>


typedef struct X86StackVar {
    uint64_t offset;
    uint32_t id;
    size_t size;
} X86StackVar;

typedef struct Stack {
    uint64_t off;

    size_t size;
    X86StackVar** vars;
} X86Stack;

void x86_push(X86Stack* stack, size_t offset, uint32_t id, uint64_t size);
void x86_pop(X86Stack* stack);
void x86_spop(X86Stack* stack);

#endif // X86_STACK_H