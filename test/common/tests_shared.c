/**************************************************************************
 *
 * Copyright (c) 2020 Alexis Lopez Zubieta <contact@azubieta.net>
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tests_shared.h"

void assert_str_list_eq(char *const *str_list_1, char *const *str_list_2) {
    char *const *itr1 = str_list_1;
    char *const *itr2 = str_list_2;
    for (; *itr1 != NULL && *itr2 != NULL; itr1++, itr2++)
        assert_str_eq(*itr1, *itr2);

    if (*itr1 != NULL || *itr2 != NULL)
        bailout();
}

void assert_str_eq(const char *str1, const char *str2) {
    if (str1 == NULL && str2 == NULL)
        return;

    if ((str1 == NULL) || (str2 == NULL)) {
        fprintf(stderr, "assert error: strings differ: %s  %s\n", str1, str2);
        bailout();
    }


    if (strcmp(str1, str2) != 0) {
        fprintf(stderr, "assert error: strings differ: %s  %s\n", str1, str2);
        bailout();
    }
}

void bailout() {
    fprintf(stdout, "FAIL\n");
    exit(1);
}

void assert_command_succeed(int ret) {
    if (ret != 0)
        bailout();
}

void assert_command_fails(int ret) {
    if (ret == 0)
        bailout();
}

void set_private_env(char const *name, char const *value) {
    setenv(name, value, 1);

    unsigned int original_var_name_size = strlen(name) + strlen(APPRUN_ENV_ORIG_PREFIX) + 1;
    char *original_var_name = calloc(original_var_name_size, sizeof(char));

    strcat(original_var_name, APPRUN_ENV_ORIG_PREFIX);
    strcat(original_var_name, name);

    setenv(original_var_name, "", 1);
    free(original_var_name);

    unsigned startup_var_name_size = strlen(name) + strlen(APPRUN_ENV_STARTUP_PREFIX) + 1;
    char *startup_var_name = calloc(startup_var_name_size, sizeof(char));

    strcat(startup_var_name, APPRUN_ENV_STARTUP_PREFIX);
    strcat(startup_var_name, name);

    setenv(startup_var_name, value, 1);
    free(startup_var_name);
}


void print_string_list(char **string_list) {
    char **itr = string_list;
    while (*itr != NULL) {
        printf("%s\n", *itr);
        itr++;
    }
}

void assert_env_item_eq(apprun_env_item_t *a, apprun_env_item_t *b) {
    if (a == NULL && b == NULL)
        return;

    if ((a == NULL) || (b == NULL))
        bailout();

    assert_str_eq(a->name, b->name);
    assert_str_eq(a->startup_value, b->startup_value);
    assert_str_eq(a->current_value, b->current_value);
    assert_str_eq(a->original_value, b->original_value);
}
