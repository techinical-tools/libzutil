#include "../../include/utlz/crypto/csprng.h"

// crypto_csprng.c
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

char* to_hex(const unsigned char *buf, size_t buf_len) {
    if (!buf || buf_len == 0) return NULL;

    // Allocate: each byte -> 2 hex chars + null terminator
    char *hex = malloc(buf_len * 2 + 1);
    if (!hex) return NULL;

    const char hex_chars[] = "0123456789ABCDEF";

    for (size_t i = 0; i < buf_len; i++) {
        hex[i*2]   = hex_chars[(buf[i] >> 4) & 0xF];
        hex[i*2+1] = hex_chars[buf[i] & 0xF];
    }

    hex[buf_len*2] = '\0';  // Null-terminate
    return hex;
}


// Returns 0 on success, -1 on failure
int csprng_get_bytesOS(uint8_t *buf, size_t len) {
    if (!buf || len == 0) return -1;

#ifdef _WIN32
    if (BCryptGenRandom(NULL, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0)
        return -1;
#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return -1;

    size_t read_bytes = fread(buf, 1, len, f);
    fclose(f);

    if (read_bytes != len) return -1;
#endif

    return 0;
}

/*hex buffer must be:
char name[UNPRINTABLE_ASCII_STRING_LEN * 2 + 1]; // for 512 UNPRINTABLE each ascii is 2bytes so mult by 2 and 
add 1 for null-terminator
*/
char* csprng_get_hexbytesOS(size_t raw_len) {
    if (raw_len == 0) return NULL;

    unsigned char *raw = malloc(raw_len);
    if (!raw) return NULL;

    if (csprng_get_bytesOS(raw, raw_len) != 0) {
        free(raw);
        return NULL;
    }

    char *hex = to_hex(raw, raw_len);
    
    free(raw); // Clean up the raw bytes
    return hex;
}
