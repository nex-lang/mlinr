#include "x86/stack.h"

#include <stdlib.h>
#include <stdio.h>
#include "x86/stack.h"

void x86_push(X86Stack* stack, size_t offset, uint32_t id, uint64_t size) {
    X86StackVar* var = malloc(sizeof(X86StackVar));
    if (var == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for stack variable\n");
        exit(EXIT_FAILURE);
    }

    stack->off -= size;

    var->offset = stack->off;
    var->id = id;
    var->size = size;

    stack->size += 1;

    X86StackVar** new_vars = realloc(stack->vars, sizeof(X86StackVar*) * stack->size);
    if (new_vars == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed for stack variables\n");
        free(var);
        exit(EXIT_FAILURE);
    }

    stack->vars = new_vars;
    stack->vars[stack->size - 1] = var;
}

void x86_pop(X86Stack* stack) {
    if (stack->size == 0) {
        fprintf(stderr, "Error: Attempting to pop from an empty stack\n");
        return;
    }

    X86StackVar* var = stack->vars[stack->size - 1];
    free(var);

    stack->size -= 1;

    stack->vars = realloc(stack->vars, sizeof(X86StackVar) * stack->size);
    if (stack->vars == NULL && stack->size > 0) {
        fprintf(stderr, "Error: Memory reallocation failed\n");
        exit(EXIT_FAILURE);
    }

    stack->off += var->size;
}

void x86_spop(X86Stack* stack) {
    if (stack->size == 0) {
        fprintf(stderr, "Error: Attempting to spop an empty stack\n");
        return;
    }

    for (size_t i = 0; i < stack->size; i++) {
        free(stack->vars[i]);
        stack->size = 0;
        stack->off = 0;
    }
}
