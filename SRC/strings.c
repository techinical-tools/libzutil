#include "../include/utlz/strings.h"

#include <string.h>
#include <stddef.h>

/**
 * Reverses a string in-place.
 * @param str The string to reverse.
 * @return Returns the pointer to the reversed string.
 */
char* strirev(char *str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    if (len <= 1) return str;

    char *start = str;
    char *end = str + len - 1;
    char temp;

    while (start < end) {
        // Swap characters
        temp = *start;
        *start = *end;
        *end = temp;

        // Move pointers toward the middle
        start++;
        end--;
    }
    return str;
}