#include "io.h"

#include <stdio.h>
#include <stdlib.h>

char* io_load_file(char* filename) {
    /*
    reads provided filename (from the directory where executable was ran / command was called)
    return: processed buffer: char* including all contents of provided filename
    */
    
    char* buffer = 0;
    __uint128_t length;
    FILE* f = fopen(filename, "rb");
    
    if (!f) {
        exit(EXIT_FAILURE);
        return NULL; 
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(sizeof(char) * (length + 1));

    if (!buffer) {
        fclose(f);
        exit(EXIT_FAILURE);
        return NULL;
    }
    
    size_t len =  fread(buffer, 1, length, f);
    fclose(f);

    buffer[len + 1] = '\0';

    return buffer;
}


