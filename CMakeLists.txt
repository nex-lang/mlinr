cmake_minimum_required(VERSION 3.20)
project(MLINR)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(GENERAL_SOURCES
    src/io.c
    src/lexer.c
    src/ast.c
    src/symtbl.c
    src/parser.c
    src/gen.c      
    src/main.c
)

set(GENERAL_HEADERS
    include/io.h
    include/bit128.h
    include/token.h
    include/lexer.h
    include/ast.h
    include/symtbl.h
    include/parser.h
    include/gen.h      
)

set(X86_SOURCES
    src/x86/x86.c
    src/x86/stack.c
    src/x86/instr.c
    src/x86/arthlogic.c
)

set(X86_HEADERS
    include/x86/x86.h
    include/x86/register.h
    include/x86/stack.h
    include/x86/mem.h
    include/x86/instr.h
    include/x86/arthlogic.h
)

set(ARM_SOURCES
    src/arm/arm.c
)

set(ARM_HEADERS
    include/arm/arm.h
)

set(RISCV_SOURCES
    src/riscv/riscv.c
)

set(RISCV_HEADERS
    include/riscv/riscv.h
)

set(SOURCES ${GENERAL_SOURCES} ${X86_SOURCES} ${ARM_SOURCES} ${RISCV_SOURCES})
set(HEADERS ${GENERAL_HEADERS} ${X86_HEADERS} ${ARM_HEADERS} ${RISCV_HEADERS})

include_directories(include)
include_directories(include/x86)
include_directories(include/arm)
include_directories(include/riscv)

add_executable(mlinr ${SOURCES} ${HEADERS})
