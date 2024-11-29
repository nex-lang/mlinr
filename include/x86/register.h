#ifndef X86_REGISTER_H
#define x86_REGISTER_H

#include "stdint.h"


#include <stdint.h>

typedef enum {
    GENERAL_PURPOSE = 1,
    SPECIAL_PURPOSE = 0
} register_type_t;

static const struct {
    const char* name;
    register_type_t type;
    const char* flag;
} x86_registers[] = {
    {"rax", GENERAL_PURPOSE, "a"},
    {"rbx", SPECIAL_PURPOSE, "b"},
    {"rcx", GENERAL_PURPOSE, "c"},
    {"rdx", GENERAL_PURPOSE, "d"},
    {"rsi", GENERAL_PURPOSE, "e"},
    {"rdi", GENERAL_PURPOSE, "f"},
    {"rbp", SPECIAL_PURPOSE, "g"},
    {"rsp", SPECIAL_PURPOSE, "h"},
    {"r8",  GENERAL_PURPOSE, "r8"},
    {"r9",  GENERAL_PURPOSE, "r9"},
    {"r10", GENERAL_PURPOSE, "r10"},
    {"r11", GENERAL_PURPOSE, "r11"},
    {"r12", SPECIAL_PURPOSE, "r12"},
    {"r13", SPECIAL_PURPOSE, "r13"},
    {"r14", SPECIAL_PURPOSE, "r14"},
    {"r15", SPECIAL_PURPOSE, "r15"}
};

#endif // X86_REGISTER_H