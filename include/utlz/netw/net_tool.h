#ifndef __NETTOOL_H__
#define __NETTOOL_H__

char* dnsresolv(const char* host, int isV4);

int net_init(void);
void net_cleanup(void);

#endif