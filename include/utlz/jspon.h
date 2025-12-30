#ifndef __JSPON_H__
#define __JSPON_H__

#include <stdlib.h>

typedef int id_t;

typedef struct {
    const char* jsonfile;
    id_t unique_id;
    unsigned int current_depth; 
    unsigned int depth;
    size_t file_size;
} json_t;

json_t init_json(json_t* json_struct, const char* json_file);
int jspon_find_value(json_t* json, const char* target_key, char** value_out);
char* load_json_file(const char* filename, size_t* out_size);

/* The 'array' is a list of tokens. 
   If array[1] is the data, we should ensure the user knows 
   how many elements were parsed to avoid OOB (Out of Bounds) access.
*/
int parse(json_t* json_struct, char** array, int max_tokens);

#endif