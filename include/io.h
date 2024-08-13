#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>


char* io_load_file(char* filename);
char* get_basename(char* path);
char* get_tempname(char* filename);

#define WO(fp, indent, format, ...) \
    do { \
        for (int i = 0; i < (indent); i++) fprintf((fp), "\t"); \
        fprintf((fp), (format), ##__VA_ARGS__); \
    } while (0)

#define EXEC(cmd, ...) \
    do { \
        char command[256]; \
        snprintf(command, sizeof(command), (cmd), ##__VA_ARGS__); \
        printf("Executing: %s\n", command); \
        int ret = system(command); \
        if (ret != 0) { \
            fprintf(stderr, "Error executing command: %s\n", command); \
        } \
    } while (0)

#endif // IO_H