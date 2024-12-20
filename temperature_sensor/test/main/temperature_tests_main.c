#include "unity.h"
#include <stdio.h>
#include <string.h>
//#include "fff.h"

static void print_banner(const char* text);

void app_main(void)
{
    print_banner("Running all the registered tests");
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}

static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}