#include "../../include/utlz/netw/net_tool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- PLATFORM HEADERS --- */
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
#endif


int net_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 0;
    }
#endif
    return 1;
}

void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}



char* dnsresolv(const char* host, int isV4) {
    if (!host) return NULL;

    struct addrinfo hints;
    struct addrinfo* res = NULL;

    memset(&hints, 0, sizeof(hints));
    if (isV4 == 0) {
        hints.ai_family   = AF_UNSPEC;
    } else if (isV4 == 1) {
        hints.ai_family   = AF_INET;
    } else if (isV4 == 2) {
        hints.ai_family   = AF_INET6;
    } else {
        return "err_ipv4_no_specified";
    } 

    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host, NULL, &hints, &res);
    if (rc != 0) {
#ifdef _WIN32
        fprintf(stderr, "DNS error: %s\n", gai_strerrorA(rc));
#else
        fprintf(stderr, "DNS error: %s\n", gai_strerror(rc));
#endif
        return NULL;
    }

    char buf[NI_MAXHOST];
    rc = getnameinfo(
        res->ai_addr,
        (socklen_t)res->ai_addrlen,
        buf,
        sizeof(buf),
        NULL,
        0,
        NI_NUMERICHOST
    );

    freeaddrinfo(res);

    if (rc != 0) {
#ifdef _WIN32
        fprintf(stderr, "Name resolution error: %s\n", gai_strerrorA(rc));
#else
        fprintf(stderr, "Name resolution error: %s\n", gai_strerror(rc));
#endif
        return NULL;
    }
#ifdef _WIN32
    net_cleanup();
#endif
    return strdup(buf);   // malloc’d, portable
}