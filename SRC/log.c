#include "../include/utlz/log.h"
#include <stdio.h>
#include <string.h>


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

int yes(void* characters) {
    char* str = (char*)characters;
    if (str == NULL || strlen(str) == 0) {
        for (;;) {
            printf("y\n");
        }
        return 0; // Note: This line is never reached because of the infinite loop
    }

    for (;;) {
        printf("%s\n", str);
    }

    return 1; // Never reached
}


void lgPrintConsole(lglevel_t log_level, char* message) {
    if (log_level == ERROR)
    {
        printf("[ERROR]: %s", message);
    } else if (log_level == INFO) {
        printf("[INFO]: %s", message);
    } else if(log_level == CRITICAL) {
        printf("[CRITICAL]: %s", message);
    } else if (log_level == WARNING) {
        printf("[WARNING]: %s", message);
    } else if(log_level == DEBUG) {
        printf("[DEBUG]: %s", message);
    } else if (log_level == CHRASH) {
        printf("[CHRASH]: %s", message);
    }   
}

void lgLogFile(lglevel_t log_level, char* message, const char* file_path) {
    if (file_path == NULL || message == NULL) return;

    // Open file in Append mode ("a")
    FILE *fptr = fopen(file_path, "a");

    if (fptr == NULL) {
        return; 
    }

    const char* level_str;
    switch (log_level) {
        case ERROR:    level_str = "ERROR";    break;
        case INFO:     level_str = "INFO";     break;
        case CRITICAL: level_str = "CRITICAL"; break;
        case WARNING:  level_str = "WARNING";  break;
        case DEBUG:    level_str = "DEBUG";    break;
        case CHRASH:   level_str = "CRASH";   break;
        default:       level_str = "UNKNOWN";  break;
    }

    // Write formatted output
    fprintf(fptr, "[%s]: %s\n", level_str, message);

    // Flush and close
    fclose(fptr);
}