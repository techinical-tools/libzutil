#include "../include/utlz/test.h"
#include<stdio.h>
#include<stdarg.h>

int testFunction(int Return_type, ...) {
    va_list args;
    va_start(args, Return_type);

    int result = -1; // Default to fail

    if (Return_type == INT_RET) {
        int actual = va_arg(args, int);
        int expected = va_arg(args, int);
        
        if (actual == expected) result = 0;
        else printf("[TEST FAIL]: Expected %d, got %d\n", expected, actual);
    } 
    else if (Return_type == CHAR_RET) {
        // Note: 'char' is promoted to 'int' when passed through '...'
        char actual = (char)va_arg(args, int);
        char expected = (char)va_arg(args, int);
        
        if (actual == expected) result = 0;
        else printf("[TEST FAIL]: Expected '%c', got '%c'\n", expected, actual);
    }

    va_end(args);
    return result;
}