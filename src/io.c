#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

char* get_basename(char* path) {
    char *base = strrchr(path, '/');
#ifdef _WIN32
    if (!base) base = strrchr(path, '\\');
#endif
    return base ? base + 1 : path;
}

char* get_tempname(char* filename) {
    char *base_filename = get_basename(filename);
    
    char temp_dir[256];
#ifdef _WIN32
    GetTempPathA(256, temp_dir);
#else
    snprintf(temp_dir, sizeof(temp_dir), ".tmp/");
    // use tmp/ instead of /tmp/ for dev purposes
    // you have to mkdir tmp/
#endif

    char *temp_filename = malloc(512 * sizeof(char));
    if (temp_filename == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    snprintf(temp_filename, 512, "%s%s", temp_dir, base_filename);

    char *ext = strstr(temp_filename, ".inr");
    if (ext && strcmp(ext, ".inr") == 0) {
        strcpy(ext, ".asm");
    }

    return temp_filename;
}

