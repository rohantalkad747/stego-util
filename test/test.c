#include "../include/minunit.h"
#include "../include/stego.h"
#include <stdio.h>
#include <string.h>


#define IMAGE_FILE "examples/bird.jpg"
#define TARGET_FILE "examples/trgt.jpg"
#define EXPECTED_TEXT "my text"

int tests_run = 0;


static char * test_e2e() {
    remove(TARGET_FILE);
    encode_driver("my text", IMAGE_FILE, TARGET_FILE);
    char* decoded = decode_driver(TARGET_FILE);
    mu_assert("error, decoded != my text", strcmp(EXPECTED_TEXT, decoded) == 0);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_e2e);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}


