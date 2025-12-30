#ifndef __XORUTIL_H__
#define __XORUTIL_H__


char* xor_hash(char* data, int repeat);
char* xorshal_hash(const char* data, const char* salt, int iterations);
int xor_shalhash_verif(const char* raw_input, const char* salt, int iterations, const char* expected_hash);
char* Xsha256(const char* data, const char* salt);

#endif