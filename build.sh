#!/bin/bash

GREEN=$(tput setaf 2)
LIME_YELLOW=$(tput setaf 190)
MAGENTA=$(tput setaf 5)
BRIGHT=$(tput bold)
NORMAL=$(tput sgr0)
UNDERLINE=$(tput smul)

CURRENT_VERSION=$(cat version.txt)
BUILD_TESTS=OFF

if [ "$1" == "-H" ] || [ "$1" == "--help" ]; then
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} Usage: ./build.sh ${LIME_YELLOW}[OPTIONS]${NORMAL} \n${UNDERLINE}Options:${NORMAL}\n  ${BRIGHT}-H, --help${NORMAL}           Display this message [mitigates all options]\n  ${BRIGHT}-C, --clean${NORMAL}          Force clear the build directory\n  ${BRIGHT}-D, --debug${NORMAL}          Equivalent to -DCMAKE_BUILD_TYPE=Debug [see CMake Documentation]\n  ${BRIGHT}-R, --release${NORMAL}        Equivalent to -DCMAKE_BUILD_TYPE=Release [see CMake Documentation]\n"
    exit 0  
fi

if [ "$1" == "-C" ] || [ "$1" == "--clean" ]; then
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} FORCE CLEARING BUILD DIRECTORY\n"
    rm -rf build/
    rm -rf .tmp/
    exit 0
fi

if [ "$#" -lt 1 ]; then
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} INSUFFICIENT ARGUMENTS PASSED (build.sh)\n"
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} USE --help or -H flag to find a usage guide\n"
    exit 1
fi

if [ "$1" == "-D" ] || [ "$1" == "--debug" ] || [ "$2" == "-D" ] || [ "$2" == "--debug" ]; then
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} ${GREEN}LATEST VERIFIED VERSION ${CURRENT_VERSION}${NORMAL}: BUILDING IN DEBUG MODE\n"
    mkdir build
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cd build/ && make
elif [ "$1" == "-R" ] || [ "$1" == "--release" ] || [ "$2" == "-R" ] || [ "$2" == "--release" ]; then
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} LATEST RELEASE VERSION ${CURRENT_VERSION}: BUILDING IN RELEASE MODE\n"
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cd build/ && make
else
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} INVALID ARGUMENTS PASSED\n"
    printf "${MAGENTA}${BRIGHT}[BUILD.SH]${NORMAL} USE --help or -H flag to find a usage guide\n"
    exit 1
fi
