#include <math.h>
#include <stdint.h>
#include "../include/utlz/maths.h"

long long unsigned int s = 0;

int srandom(long long unsigned int key) {
    if (key == 0) {
        s = 1;
        return -1;
    }
    
    s = key;
    return 0;
}

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

long long unsigned random(int isRNG) {
    long long unsigned int result;
    if (isRNG == 1) {
        s = (s * 9301 + 49297) % 233280;
        return s;
    }

    s = (s ^ 0x5DEECE66D) ^ 0xDEADBEEF;
    s = (s ^ 0xB3B3B3B3) ^ (0x1010101 + 0xDEAD911);
    s = (s * 1103515245 + 12345);

    s += 0x9E3779B97F4A7C15;

    for (size_t i = 0; i < 10; i++)
    {
        s = rotl(s, 0xF3C7 ^ ((s * s) % 0xF1F1));
        s = rotl(s, 9301);
    }

    s += 0x9E3779B97F4A7C15;

    s = rotl(s, 9308931);

    s += 0x9E3779B97F4A7C15;

    s = s + (s >> 32) ^ (s & 0xFFFFFFFF);
    result = s;
    result = result & RNG_MAX; // defined in maths.h 

    return result;
}