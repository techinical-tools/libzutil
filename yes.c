#include "include/utlz/log.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    // Note: argc is 1 if only the program name is run. 
    // argv[1] only exists if argc > 1.
    if (argc > 1) {
        char buffer[256];
        // Use snprintf to safely combine the string and the argument
        snprintf(buffer, sizeof(buffer), "Running with: %s", argv[1]);
        
        lgPrintConsole(INFO, buffer);
        
        // Pass the argument to your yes function
        yes((void*)argv[1]);
    } else {
        lgPrintConsole(WARNING, "No arguments provided. Defaulting to 'y' loop.");
        yes(NULL);
    }

    return 0;
}
