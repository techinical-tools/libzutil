#include "../include/utlz/jspon.h"
#include <stdio.h>
#include <stdlib.h>

char* load_json_file(const char* filename, size_t* out_size) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    // Move to end to find file size
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer (+1 for null terminator)
    char* buffer = (char*)malloc(size + 1);
    if (buffer) {
        fread(buffer, 1, size, f);
        buffer[size] = '\0';
        if (out_size) *out_size = size;
    }

    fclose(f);
    return buffer;
}