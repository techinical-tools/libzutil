#include "include/utlz/crypto/xor_util.h"
#include "include/utlz/log.h"
#include "include/utlz/test.h"
#include <stdio.h>
#include <stdlib.h>
#include "include/utlz/maths.h"
#include "include/utlz/strings.h"
#include "include/utlz/crypto/basexx.h"
#include "include/utlz/crypto/csprng.h"
#include "include/utlz/netw/net_tool.h"


#define ITER 10
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
    net_init(); // call if using windwos
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

    srandom(885);

    int i = 0;
    while (i < 1000)
    {
        printf("rng: %lld ", random(0));
        printf("prng: %lld\n", random(PRNG));
        i++;
    }
    
    char rndling[512];  // allocate 512 bytes
    int ret = csprng_get_bytesOS(rndling, sizeof(rndling));
    if (ret != 0) {
        printf("Error getting random bytes: %d\n", ret);
    } else {
        // Print raw bytes safely as hex, since it's not a string
        for (int i = 0; i < sizeof(rndling); i++) {
            printf("%02X", (unsigned char)rndling[i]);
        }
        printf("\n");
    }

    size_t bytes_len = 512; // number of raw bytes
    char *hex = csprng_get_hexbytesOS(bytes_len);
    if (hex) {
        printf("%s\n", hex);
        free(hex);
    }

    char stralot[] = "hello world";
    printf("\nreversed string of %s is:", stralot);
    strirev(stralot);
    printf(" %s\n", stralot);
    
    printf("\nencoding of 128_en 'hi': %s  128_de: %s", base128_en("hi"), base128_de(base128_en("hi")));
    
    printf("xor_hash(stralot, 100) of %s is: ", stralot);
    xor_hash(stralot, 100);
    printf("%s\n", base64_en(stralot));

    const char* url = "transact_879274# hacker<-google link:$https://transactserver1.bank-of-XSS.com/?user=google-account/?command=send_to=hacker/?command=setvar.amount=(DOLLAR_SIGNLcommand=./get_account_money.shDOLLAR_SIGNL(./add_space.sh)DOLLAR_SIGNL1=google-account) ";
    char* gethackedlol = xorshal_hash(url, "XSS-BANK", ITER);
    printf("char gethackedlol[] is: %s\n ", gethackedlol);

    if(xor_shalhash_verif(url, "XSS-BANK", ITER, "ef/PPXbdef/") == 1) {
        printf("string verif: passed! (xor_verif passed)\n");
    } else {
        printf("string verif: failed! (xor_verif passed)\n");
    }

    if(xor_shalhash_verif(url, "XSS-BANK ILY TEAM", ITER, "ef/PPXbdef/") == 1) {
        printf("string verif: passed! (xor_verif passed)\n");
    } else {
        printf("string verif: failed! (xor_verif passed)\n");
    }

    char* ipaddrc = dnsresolv("google.com", 1);
    printf("ip addr for google.com: %s\n", ipaddrc);

    free(ipaddrc);
    net_cleanup(); // for windows
    return 0;
}
