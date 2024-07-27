cmake_minimum_required(VERSION 3.20)
project(MLINR)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCES
    src/main.c
    src/x86.c
)

set(HEADERS
    include/x86/syscall.h
    include/x86/register.h
)

include_directories(include)
add_executable(nex ${SOURCES} ${HEADERS})