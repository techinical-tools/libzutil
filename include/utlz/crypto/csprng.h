#ifndef __CSPRNG_H__
#define __CSPRNG_H__

#include<stdint.h>
#include<stddef.h>

int csprng_get_bytesOS(uint8_t *buf, size_t len);
char* csprng_get_hexbytesOS(size_t raw_len);

#endif