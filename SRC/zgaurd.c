#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#endif



void __attribute__((constructor(101))) zutil_security_init(void) {
    // 1. Environment Sanity Check
    // Check if we are being debugged or if LD_PRELOAD is being tampered with

    #ifndef _WIN32
    if (getenv("LD_PRELOAD") != NULL) {
        fprintf(stderr, "[Z-GUARD] Security Alert: Unauthorized library injection detected!\n");
        exit(1);
    }
    #endif

    #ifdef __linux__
    uid_t euid;

    /// get uid.
    euid = geteuid();
    #ifndef _NEED_SUDO
        if (euid == 0) {
            printf("[Z-GAURD]: security breach 'running as sudo' _NEED_SUDO \nshuold be defined in compile time")
        }
    #endif

    #elif defined(_WIN32)
    #ifndef _NEED_SUDO

    #endif
    #endif
}