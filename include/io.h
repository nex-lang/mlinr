#ifndef IO_H
#define IO_H

char* io_load_file(char* filename);
char* get_basename(char* path);
char* get_tempname(char* filename);

#define WO(fp, indent, format, ...) \
    do { \
        for (int i = 0; i < (indent); i++) fprintf((fp), "\t"); \
        fprintf((fp), (format), ##__VA_ARGS__); \
    } while (0)


#endif // IO_H