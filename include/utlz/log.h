#ifndef __LOG_H__
#define __LOG_H__

#include "ltype.h"

#define ERROR 0
#define INFO 1
#define CRITICAL 2
#define WARNING 3
#define DEBUG 4
#define CHRASH -1

void lgPrintConsole(lglevel_t log_level, char* message);
void lgLogFile(lglevel_t log_level, char* message, const char* file_path);
int yes(void* characters);

#endif