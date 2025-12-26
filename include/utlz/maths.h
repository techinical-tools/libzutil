#ifndef __MATHS_H__
#define __MATHS_H__

#define RNG_MAX 0x7FFFFFFF

#ifdef LEGACY
    #define RNG 1
#else
    #define RNG 0
#endif

#define PRNG 0

int srandom(long long unsigned int key);
long long unsigned int random(int isRNG);

#endif