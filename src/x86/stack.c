#include "x86/stack.h"

#include <stdlib.h>
#include <stdio.h>
#include "x86/stack.h"

X86Stack* x86_stack() {
    X86Stack* stack = malloc(sizeof(X86Stack));

    stack->off = 0;
    stack->size = 0;
    stack->lsize = 0;
    stack->vars = malloc(sizeof(X86StackVar));

    return stack;
}

void x86_push(X86Stack* stack, size_t offset, int32_t id, uint64_t size) {
    X86StackVar* var = malloc(sizeof(X86StackVar));
    if (var == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for stack variable\n");
        exit(EXIT_FAILURE);
    }

    stack->off = stack->size - size;

    var->offset = stack->off;
    var->id = id;
    var->size = size;
    
    stack->lsize += 1;

    X86StackVar** new_vars = realloc(stack->vars, sizeof(X86StackVar*) * stack->lsize);
    if (new_vars == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed for stack variables\n");
        free(var);
        exit(EXIT_FAILURE);
    }

    stack->vars = new_vars;
    stack->vars[stack->lsize - 1] = var;
}

void x86_pop(X86Stack* stack) {
    if (stack->lsize == 0) {
        fprintf(stderr, "Error: Attempting to pop from an empty stack\n");
        return;
    }

    X86StackVar* var = stack->vars[stack->lsize - 1];
    free(var);

    stack->lsize -= 1;

    stack->vars = realloc(stack->vars, sizeof(X86StackVar) * stack->lsize);
    if (stack->vars == NULL && stack->lsize > 0) {
        fprintf(stderr, "Error: Memory reallocation failed\n");
        exit(EXIT_FAILURE);
    }

    stack->off += var->size;
}

void x86_spop(X86Stack* stack) {
    if (stack->lsize == 0) {
        fprintf(stderr, "Error: Attempting to spop an empty stack\n");
        return;
    }

    for (size_t i = 0; i < stack->lsize; i++) {
        free(stack->vars[i]);
        stack->lsize = 0;
        stack->off = 0;
    }
}

size_t x86_lookup_size(X86Stack* stack, int32_t id) {
    for (size_t i = 0; i < stack->lsize; i++) {
        if (stack->vars[i]->id == id) {
            return stack->vars[i]->size;
        }
    }

    return -1;
}

uint64_t x86_lookup_offset(X86Stack* stack, int32_t id) {
    for (size_t i = 0; i < stack->lsize; i++) {
        if (stack->vars[i]->id == id) {
            return stack->vars[i]->offset;
        }
    }

    fprintf(stderr, "Error: Try to acess non-existant variable\n");

    return 0;
}