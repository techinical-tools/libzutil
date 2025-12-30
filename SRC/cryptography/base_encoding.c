#include "../../include/utlz/crypto/basexx.h"

static const char b128_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/_-;:'<,.?!@#$^&*()~`";  // works on 28bit level

static const int8_t b128_inv_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//  sp  !   "   #   $   %   &   '   (   )   * +   ,   -   .   /
    -1, 76, -1, 78, 79, -1, 81, 66, 82, 83, 80, 62, 69, 64, 70, 63,
//  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 67, 65, 68, -1, -1, 77,
//  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    75,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
//  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, 84, 64,
//  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    86, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
//  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   del
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, 85, -1,
    /* ... rest are -1 ... */
};

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char b32_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Base64 Inverse Table
static const int8_t b64_inv[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

// Base32 Inverse Table (A-Z, 2-7)
static const int8_t b32_inv[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1
};

// --- Base64 Encoding ---
char* base64_en(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    size_t out_len = 4 * ((len + 2) / 3);
    char* out = (char*)malloc(out_len + 1);
    if (!out) return NULL;

    for (size_t i = 0, j = 0; i < len; ) {
        uint32_t a = (unsigned char)string[i++];
        uint32_t b = (i < len) ? (unsigned char)string[i++] : 0;
        uint32_t c = (i < len) ? (unsigned char)string[i++] : 0;
        uint32_t combined = (a << 16) | (b << 8) | c;

        out[j++] = b64_table[(combined >> 18) & 0x3F];
        out[j++] = b64_table[(combined >> 12) & 0x3F];
        out[j++] = (i > len + 1) ? '=' : b64_table[(combined >> 6) & 0x3F];
        out[j++] = (i > len) ? '=' : b64_table[combined & 0x3F];
    }
    out[out_len] = '\0';
    return out;
}

// --- Base32 Encoding ---
char* base32_en(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    // 8 chars for every 5 bytes
    size_t out_len = ((len + 4) / 5) * 8;
    char* out = (char*)malloc(out_len + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (i < len) {
        uint64_t buffer = 0;
        int count = 0;
        // Pack up to 5 bytes into a 64-bit buffer
        for (int b = 0; b < 5; b++) {
            buffer <<= 8;
            if (i < len) {
                buffer |= (unsigned char)string[i++];
                count++;
            }
        }

        // Shift the buffer to align the first 5-bit chunk to the top of the relevant bits
        int bits = count * 8;
        int pad = (5 - count) * 8; 
        buffer <<= pad;

        for (int chunk = 0; chunk < 8; chunk++) {
            if (chunk * 5 < bits) {
                int shift = 35 - (chunk * 5);
                out[j++] = b32_table[(buffer >> shift) & 0x1F];
            } else {
                out[j++] = '='; // Padding
            }
        }
    }
    out[out_len] = '\0';
    return out;
}


// Base64: 4 chars -> 3 bytes
char* base64_de(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    char* out = malloc((len / 4 * 3) + 1);
    size_t i = 0, j = 0;

    while (i < len) {
        uint32_t v = 0;
        for (int k = 0; k < 4; k++) {
            v <<= 6;
            if (string[i] != '=') v |= b64_inv[(unsigned char)string[i]];
            i++;
        }
        out[j++] = (v >> 16) & 0xFF;
        if (string[i-2] != '=') out[j++] = (v >> 8) & 0xFF;
        if (string[i-1] != '=') out[j++] = v & 0xFF;
    }
    out[j] = '\0';
    return out;
}

// Base32: 8 chars -> 5 bytes
char* base32_de(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    char* out = malloc((len / 8 * 5) + 1);
    size_t i = 0, j = 0;

    while (i < len) {
        uint64_t v = 0;
        for (int k = 0; k < 8; k++) {
            v <<= 5;
            if (string[i] != '=') v |= (b32_inv[(unsigned char)string[i]] & 0x1F);
            i++;
        }
        for (int b = 4; b >= 0; b--) {
            uint8_t byte = (v >> (b * 8)) & 0xFF;
            if (byte != 0 || j < (len/8*5)) out[j++] = byte;
        }
    }
    out[j] = '\0';
    return out;
}

char* base128_en(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    
    // 8 characters out for every 7 bytes in
    size_t out_len = ((len + 6) / 7) * 8;
    char* out = malloc(out_len + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (i < len) {
        uint64_t block = 0;
        int count = 7;
        // Pack bytes into the block from left to right
        for (int b = 0; b < count; b++) {
            block = (block << 8) | (unsigned char)string[i++];
        }

        // Now extract 7-bit chunks from the TOP of the data we just put in
        int bits_to_send = count * 8;
        for (int chunk = 0; chunk < 8; chunk++) {
            int shift = (7 * 8) - 7 - (chunk * 7); // Start at bit 49
            if (chunk * 7 < bits_to_send) {
                out[j++] = b128_table[(block >> shift) & 0x7F];
            } else {
                out[j++] = '=';
            }
        }
    }
    out[j] = '\0';
    return out;
}

char* base128_de(const char* string) {
    if (!string) return NULL;
    size_t len = strlen(string);
    
    // Allocate space: 8 chars encode 7 bytes.
    char* out = malloc(((len + 7) / 8 * 7) + 1);
    if (!out) return NULL;
    
    size_t i = 0, j = 0;

    while (i < len) {
        uint64_t block = 0;
        int char_count = 0;

        // 1. Pack exactly 8 characters into a 56-bit block
        for (int k = 0; k < 8; k++) {
            block <<= 7; // Make room for next 7 bits
            if (i < len && string[i] != '=') {
                int8_t val = b128_inv_table[(unsigned char)string[i++]];
                if (val != -1) {
                    block |= (val & 0x7F);
                    char_count++;
                }
            } else if (i < len && string[i] == '=') {
                i++; // Skip padding
            }
        }

        // 2. Extract the bytes. 
        // In our 56-bit block, 8 chars always represent 7 potential bytes.
        // We only extract the bytes that actually contain data.
        int bytes_to_extract = (char_count * 7) / 8;
        
        for (int b = 0; b < bytes_to_extract; b++) {
            // Shift down from the 56-bit total (8 * 7 = 56)
            // The first byte is at (56 - 8), second at (56 - 16), etc.
            int shift = 48 - (b * 8); 
            out[j++] = (block >> shift) & 0xFF;
        }
    }

    out[j] = '\0';
    return out;
}

char* base128_en_raw(const uint8_t* input, size_t len) {
    if (!input || len == 0) return NULL;

    // Calculate output size: Base128 uses 7 bits per char.
    // (len * 8 bits) / 7 bits per char, rounded up + 1 for null terminator.
    size_t out_len = ((len * 8) + 6) / 7; 
    char* output = (char*)malloc(out_len + 1);
    
    size_t bit_acc = 0;   // Bit accumulator
    int bits_in_acc = 0;  // Count of bits in accumulator
    size_t out_idx = 0;
    size_t in_idx = 0;

    while (in_idx < len || bits_in_acc > 0) {
        // Pull in more bits from the raw input
        if (bits_in_acc < 7 && in_idx < len) {
            bit_acc |= (size_t)input[in_idx++] << bits_in_acc;
            bits_in_acc += 8;
        }

        // Take 7 bits at a time and map them to our Non-XSS charset
        uint8_t seven_bits = bit_acc & 0x7F;
        output[out_idx++] = b128_table[seven_bits];
        
        bit_acc >>= 7;
        bits_in_acc -= 7;

        // Safety break for the end of the raw buffer
        if (in_idx == len && bits_in_acc <= 0) break;
    }

    output[out_idx] = '\0';
    return output;
}