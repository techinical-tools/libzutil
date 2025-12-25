#include "include/utlz/log.h"
#include "include/utlz/test.h"

int my_add(int a, int b) { return a + b; }

void suite1() {
    int val = my_add(2, 2);
    
    // Test if 2+2 = 4
    if (testFunction(INT_RET, val, 4) != 0) {
        lgLogFile(ERROR, "Addition test failed!", "tests.log");
    } else {
        lgLogFile(INFO, "Addition test passed.", "tests.log");
    }
}

void suite2() {
    int val = my_add(2, 2);
    
    // Test if 2+2 = 4
    if (testFunction(INT_RET, val, 1337) != 0) {
        lgLogFile(ERROR, "Addition test failed!", "tests.log");
    } else {
        lgLogFile(INFO, "Addition test passed.", "tests.log");
    }
}

int main() {
    // init
    const char log_filepath[] = "logfile.test.log";
    /*
      test the console functions
      should pass
    */
    lgPrintConsole(ERROR, "test\n");
    lgPrintConsole(CRITICAL, "test\n");
    lgPrintConsole(DEBUG, "test\n");
    lgPrintConsole(INFO, "test\n");
    lgPrintConsole(CHRASH, "test\n");
    lgPrintConsole(WARNING, "test\n");

    lgLogFile(ERROR, "test", log_filepath);
    lgLogFile(CRITICAL, "test", log_filepath);
    lgLogFile(DEBUG, "test", log_filepath);
    lgLogFile(INFO, "test", log_filepath);
    lgLogFile(CHRASH, "test", log_filepath);
    lgLogFile(WARNING, "test", log_filepath);

    // test the Testing/unit test funcs
    suite1();
    suite2();

    return 0;
}