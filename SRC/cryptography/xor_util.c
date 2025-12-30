#include "../../include/utlz/crypto/basexx.h"
#include "../../include/utlz/crypto/xor_util.h"
#include "../../include/utlz/strings.h"
#include "../../include/utlz/crypto/csprng.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define CH(x,y,z)  (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define ROTR(x,n)  (((x) >> (n)) | ((x) << (32 - (n))))
#define SIG0(x)    (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIG1(x)    (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

char* z_base64_encode(const unsigned char* input, size_t len, char* output) {
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    size_t j = 0;
    for (size_t i = 0; i < len; i += 3) {
        uint32_t octet_a = input[i];
        uint32_t octet_b = (i + 1 < len) ? input[i + 1] : 0;
        uint32_t octet_c = (i + 2 < len) ? input[i + 2] : 0;

        uint32_t combined = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = table[(combined >> 18) & 0x3F];
        output[j++] = table[(combined >> 12) & 0x3F];
        output[j++] = (i + 1 < len) ? table[(combined >> 6) & 0x3F] : '=';
        output[j++] = (i + 2 < len) ? table[combined & 0x3F] : '=';
    }
    return output; 
}

char* z_string_shorten(char* str, size_t n) {
    if (!str) return NULL;

    size_t current_len = strlen(str);

    // If the string is already shorter than or equal to n, do nothing.
    if (current_len > n) {
        str[n] = '\0'; // Move the null terminator to index n
    }

    return str;
}

// Final combined logic for the toolbox
char* xor_hash(char* data, int repeat) {
    if (!data) return NULL;

    size_t data_len = strlen(data);
    int iterations = (repeat <= 0) ? 1 : repeat;

    // Apply the entropy loops
    for (int i = 0; i < iterations; i++) {
        char* key = csprng_get_hexbytesOS(56); 
        if (!key) return NULL;

        size_t key_len = strlen(key);
        for (size_t j = 0; j < data_len; j++) {
            data[j] ^= key[j % key_len];
        }
        free(key); 
    }

    // Prepare Base64 Output
    size_t b64_len = ((data_len + 2) / 3) * 4;
    char* b64_output = malloc(b64_len + 1);
    
    if (b64_output) {
        z_base64_encode((unsigned char*)data, data_len, b64_output);
        b64_output[b64_len] = '\0'; 
        
        // Calculate the target length dynamically
        size_t target_len;

        if (b64_len >= 64) {
            target_len = b64_len - 32;
        } else {
            target_len = b64_len / 2; 
        }

        // Edge case: if length was 1, 1/2 = 0. We might want at least 1 char.
        if (target_len == 0 && b64_len > 0) target_len = 1;

        return z_string_shorten(b64_output, target_len);
    }
    return NULL;
}

char* xorshal_hash(const char* data, const char* salt, int iterations) {
    if (!data || !salt) return NULL;
    
    size_t data_len = strlen(data);
    size_t salt_len = strlen(salt);
    
    // Copy data to a mutable buffer
    char* work_buffer = strdup(data);

    for (int i = 0; i < iterations; i++) {
        for (size_t j = 0; j < data_len; j++) {
            // XOR with the salt, indexed by position AND iteration
            // This creates a moving target for every bit
            work_buffer[j] ^= salt[(j + i) % salt_len];
        
            work_buffer[j] = (work_buffer[j] << 3) | (work_buffer[j] >> 5);
        }
    }

    char* b128_out = base128_en(work_buffer);
    free(work_buffer);
    
    return b128_out;
}

int xor_shalhash_verif(const char* raw_input, const char* salt, int iterations, const char* expected_hash) {
    if (!raw_input || !salt || !expected_hash) return 0;

    char* current_hash = xorshal_hash(raw_input, salt, iterations);

    if (!current_hash) return 0;

    int result = 1;
    size_t len_a = strlen(current_hash);
    size_t len_b = strlen(expected_hash);

    if (len_a != len_b) {
        result = 0;
    } else {
        for (size_t i = 0; i < len_a; i++) {
            if (current_hash[i] != expected_hash[i]) {
                result = 0;
            }
        }
    }

    free(current_hash);
    return result;
}

void sha256_transform(uint32_t state[8], const uint8_t data[64]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    // 1. Prepare the message schedule (m[0..15] from raw data)
    for (i = 0, j = 0; i < 16; i++, j += 4) {
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    }

    // 2. Extend the message schedule into m[16..63]
    for (i = 16; i < 64; i++) {
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

    // 3. Initialize working variables with current state
    a = state[0]; b = state[1]; c = state[2]; d = state[3];
    e = state[4]; f = state[5]; g = state[6]; h = state[7];

    // 4. THE 64 ROUNDS (The "Non-Vomit" Shredder)
    for (i = 0; i < 64; i++) {
        t1 = h + EP1(e) + CH(e, f, g) + K[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // 5. Add working variables back into state
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

void sha256_compute(const uint8_t* data, size_t len, uint8_t out_hash[32]) {
    // 1. Setup the Internal State
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // 2. Padding Logic (The Bitcoin-Standard padding)
    // We create a buffer that is a multiple of 64 bytes (512 bits)
    size_t padded_len = ((len + 8 + 64) / 64) * 64;
    uint8_t* padded_data = (uint8_t*)calloc(1, padded_len);
    
    memcpy(padded_data, data, len);
    padded_data[len] = 0x80; // The '1' bit in padding

    // Add the length in bits at the very end (Big Endian)
    uint64_t bit_len = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++) {
        padded_data[padded_len - 1 - i] = (uint8_t)(bit_len >> (i * 8));
    }

    // 3. The Main Transformation Loop
    // Process the data in 64-byte chunks
    for (size_t i = 0; i < padded_len; i += 64) {
        sha256_transform(state, &padded_data[i]);
    }

    // 4. Finalize the hash into the out_hash buffer (Big Endian)
    for (int i = 0; i < 8; i++) {
        out_hash[i * 4 + 0] = (uint8_t)(state[i] >> 24);
        out_hash[i * 4 + 1] = (uint8_t)(state[i] >> 16);
        out_hash[i * 4 + 2] = (uint8_t)(state[i] >> 8);
        out_hash[i * 4 + 3] = (uint8_t)(state[i]);
    }

    free(padded_data);
}



char* Xsha256(const char* data, const char* salt) {
    if (!data) {
        return "0xE4404"; 
    } // return error

    char* reversed_data = strdup(data);
    if (!reversed_data) return "0xE4405";

    strirev(reversed_data);

    char* salted_input = xorshal_hash((char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(char*)(int*)/*why int? also who the fuck needs arm (*apple)*/(char*)(char*)/*My code My problem*/reversed_data, salt, 256);
    free(reversed_data);

    uint8_t raw_sha[32];
    sha256_compute((uint8_t*)salted_input, strlen(salted_input), raw_sha);

    char* final_output = base128_en_raw(raw_sha, 32);
    
    free(salted_input);

    return final_output;
} // its xha cuz its eXtra Secure Hashing Algorithim

int Xsha_verify(const char* data, const char* salt, const char* expected_hash) {
    if (!data || !salt || !expected_hash) return -1;

    // 1. Generate the attempt hash
    char* attempt = Xsha256(data, salt);
    if (!attempt) return -1;

    // 2. Safety check: compare lengths first
    size_t len_attempt = strlen(attempt);
    size_t len_expected = strlen(expected_hash);
    
    // We use a volatile result to prevent the compiler from optimizing the loop
    volatile int result = 0;
    if (len_attempt != len_expected) {
        result = 1; 
    }

    for (size_t i = 0; i < len_expected; i++) {
        // If attempt is shorter, we XOR with a dummy value
        char a = (i < len_attempt) ? attempt[i] : 0;
        char b = expected_hash[i];
        result |= (a ^ b);
    }

    free(attempt);

    // Return 1 for success (result is 0), 0 for failure
    return (result == 0);
}