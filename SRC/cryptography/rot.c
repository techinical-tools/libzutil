#include <ctype.h>
#include <string.h>
#include "../../include/utlz/crypto/linerot.h"

#define MACRO_ROT(n, text) do {                                                  \
    char *s = (char *)(text); /* Cast away const to allow in-place modification */\
    size_t len = strlen(s);                                                      \
    int shift = (int)(n);                                                        \
    for (size_t i = 0; i < len; i++) {                                           \
        char c = s[i];                                                           \
        /* Handle uppercase letters A-Z */                                       \
        if (isupper(c)) {                                                        \
            s[i] = (char)(((c - 'A' + shift) % 26) + 'A');                       \
        }                                                                        \
        /* Handle lowercase letters a-z */                                       \
        else if (islower(c)) {                                                   \
            s[i] = (char)(((c - 'a' + shift) % 26) + 'a');                       \
        }                                                                        \
        /* Handle digits 0-9 */                                                  \
        else if (isdigit(c)) {                                                   \
            s[i] = (char)(((c - '0' + shift) % 10) + '0');                       \
        }                                                                        \
        /* Non-alphanumeric characters are unchanged */                          \
    }                                                                            \
} while (0)


#define MACRO_ROT47(text) do {                                                  \
    char *s = (char *)(text);                                                    \
    size_t len = strlen(s);                                                      \
    for (size_t i = 0; i < len; i++) {                                           \
        char c = s[i];                                                           \
        if (c >= '!' && c <= '~') { /* Range is ASCII 33 to 126 */               \
            s[i] = (char)((((c - '!') + 47) % 94) + '!');                        \
        }                                                                        \
    }                                                                            \
} while (0)

void ROT47(char* text) {
    MACRO_ROT47(text);
}

void ROT13(char* text) {
    MACRO_ROT(13, text);
}

void ROT(char* text, int n) {
    MACRO_ROT(n, text);
}