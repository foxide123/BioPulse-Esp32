#include "unity.h"
#include <stdio.h>
#include <string.h>

static void print_banner(const char* text);

void app_main(void)
{
    print_banner("Executing one test by its name");
    UNITY_BEGIN();
    unity_run_test_by_name("ds18b20 test example");
    UNITY_END();
}

static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}