#include "../include/utlz/jspon.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* =========================================================================
 * INTERNAL HELPER FUNCTIONS (Not exposed in .h)
 * ========================================================================= */

// Safely skips whitespace characters (Space, Tab, Newline, Carriage Return)
static char* skip_whitespace(char* cursor) {
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r') {
        cursor++;
    }
    return cursor;
}

// Finds the end of a JSON string, handling escaped quotes (\")
// Returns NULL if the string is malformed (no ending quote found)
static char* find_string_end(char* cursor) {
    // cursor points to the opening quote '"'
    cursor++; 
    while (*cursor != '\0') {
        if (*cursor == '\\') {
            // Found an escape character. Skip the next char (e.g., \" or \\)
            cursor++;
            if (*cursor != '\0') cursor++;
        } else if (*cursor == '"') {
            // Found the closing quote
            return cursor; 
        } else {
            cursor++;
        }
    }
    return NULL; // Reached End of String without finding a closing quote
}

/* =========================================================================
 * PUBLIC API IMPLEMENTATION
 * ========================================================================= */

json_t init_json(json_t* json_struct, const char* json_file) {
    json_t empty = {0};
    if (!json_struct || !json_file) return empty;

    json_struct->jsonfile = json_file;
    json_struct->depth = 0;
    json_struct->file_size = strlen(json_file);
    
    // Generate a simple XOR "checksum" ID for unique identification
    json_struct->unique_id = 0xCAFEBABE; // Seed
    size_t scan_len = json_struct->file_size > 64 ? 64 : json_struct->file_size;
    for (size_t i = 0; i < scan_len; i++) {
        json_struct->unique_id ^= (json_file[i] << (i % 8));
    }

    return *json_struct;
}

/* * MAIN PARSER
 * Fills 'array' with pointers to keys and values.
 * MODIFIES the input string buffer (In-Place Null Termination).
 */
int parse(json_t* json_struct, char** array, int max_tokens) {
    if (!json_struct || !json_struct->jsonfile || !array || max_tokens < 1) return -1;

    // We cast to char* because we MUST modify the string for in-place termination.
    // The user of this library must provide a mutable buffer.
    char* cursor = (char*)json_struct->jsonfile;
    int token_count = 0;
    
    // Safety Limit: Default to 32 if the struct wasn't configured properly
    unsigned int depth_limit = 32; 

    while (*cursor != '\0' && token_count < max_tokens) {
        cursor = skip_whitespace(cursor);

        // 1. Depth & Security Check
        if (*cursor == '{' || *cursor == '[') {
            json_struct->depth++;
            if (json_struct->depth > depth_limit) {
                // Z-GUARD: Stack Overflow protection
                return -2; 
            }
            cursor++;
            continue;
        }
        if (*cursor == '}' || *cursor == ']') {
            if (json_struct->depth == 0) {
                // Z-GUARD: Unbalanced structure
                return -3; 
            }
            json_struct->depth--;
            cursor++;
            continue;
        }

        // 2. Parse Strings (Keys and Values)
        if (*cursor == '"') {
            char* start = cursor + 1; // Content starts after the quote
            char* end = find_string_end(cursor);
            
            if (!end) return -4; // Error: Malformed string

            *end = '\0'; // Z-TRICK: Terminate string in-place
            
            array[token_count] = start; // Store the pointer
            token_count++;

            cursor = end + 1; // Move past the quote
            continue;
        }

        // 3. Parse Primitives (Booleans, Nulls) - Basic implementation
        // Check for 't' (true), 'f' (false), 'n' (null)
        if (*cursor == 't' || *cursor == 'f' || *cursor == 'n') {
             char* start = cursor;
             // Naive scan until separator
             while (*cursor >= 'a' && *cursor <= 'z') cursor++;
             
             // If we have space, null terminate manually if needed, 
             // but simpler parsers might just skip this logic or treat as string.
             // For strict JSON, we expect a separator next.
             continue;
        }
        
        // 4. Parse Numbers (Simplified)
        if ((*cursor >= '0' && *cursor <= '9') || *cursor == '-') {
             char* start = cursor;
             while ((*cursor >= '0' && *cursor <= '9') || *cursor == '.' || *cursor == 'e' || *cursor == 'E' || *cursor == '+' || *cursor == '-') {
                 cursor++;
             }
             // Store the number as a string token
             if (token_count < max_tokens) {
                 // Note: We can't easily null-terminate numbers without overwriting the separator
                 // unless the separator is a space/newline.
                 // For this "Pak" version, we often skip storing raw numbers in the token array
                 // unless specifically requested, or we rely on the user to use atoi(start).
             }
             continue;
        }

        // Skip separators
        if (*cursor == ':' || *cursor == ',') {
            cursor++;
            continue;
        }

        // If we hit unknown chars, just advance to avoid infinite loops
        cursor++;
    }

    return token_count;
}

/*
 * OPTIMIZED SEARCH
 * Scans the file for a specific key WITHOUT storing tokens.
 * Useful for reading 1 config value from a large file.
 */
int jspon_find_value(json_t* json, const char* target_key, char** value_out) {
    if (!json || !json->jsonfile || !target_key || !value_out) return 0;

    char* cursor = (char*)json->jsonfile;
    // Local depth tracking to avoid messing up the main struct state
    unsigned int current_depth = 0;
    unsigned int max_depth = 64; 

    while (*cursor != '\0') {
        cursor = skip_whitespace(cursor);

        // Track structure
        if (*cursor == '{') { current_depth++; cursor++; continue; }
        if (*cursor == '}') { if (current_depth > 0) current_depth--; cursor++; continue; }

        // Look for Keys
        if (*cursor == '"') {
            char* start = cursor + 1;
            char* end = find_string_end(cursor);
            if (!end) return -1;

            // Check match length first (Optimization)
            size_t len = end - start;
            if (len == strlen(target_key) && strncmp(start, target_key, len) == 0) {
                // KEY FOUND! Now find the value.
                cursor = end + 1;
                while (*cursor == ':' || *cursor == ' ' || *cursor == '\t') cursor++;

                if (*cursor == '"') {
                    // Value is a string
                    char* v_start = cursor + 1;
                    char* v_end = find_string_end(cursor);
                    if (v_end) {
                        *v_end = '\0'; // Terminate it so user can read it
                        *value_out = v_start;
                        return 1; // Found
                    }
                }
                // If value is number/bool, add logic here...
                return 0; // Value parsing failed or not supported type
            }

            cursor = end + 1;
            continue;
        }
        cursor++;
    }
    return 0; // Not found
}