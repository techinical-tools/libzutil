#ifndef __BASEXX_H__
#define __BASEXX_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Base64 Functions
char* base64_en(const char* string);
char* base64_de(const char* string);

// Base32 Functions
char* base32_en(const char* string);
char* base32_de(const char* string);

// Base128 functions
char* base128_de(const char* string);
char* base128_en(const char* string);
char* base128_en_raw(const uint8_t* input, size_t len);

#endif