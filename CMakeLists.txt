cmake_minimum_required(VERSION 3.20)
project(MLINR)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCES
    src/io.c
    src/lexer.c
    src/ast.c
    src/symtbl.c
    src/parser.c
    src/gen.c      
    src/main.c
    src/x86.c
)

set(HEADERS
    include/io.h
    include/bit128.h
    include/token.h
    include/lexer.h
    include/ast.h
    include/symtbl.h
    include/parser.h
    include/gen.h      

    include/x86/register.h
)

include_directories(include)
add_executable(mlinr ${SOURCES} ${HEADERS})