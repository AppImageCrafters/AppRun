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

void assert_str_list_eq(char* const* str_list_1, char* const* str_list_2) {
    char* const* itr1 = str_list_1;
    char* const* itr2 = str_list_2;
    for (; *itr1 != NULL && *itr2 != NULL; itr1++, itr2++)
        assert_str_eq(*itr1, *itr2);

    if (*itr1 != NULL || *itr2 != NULL)
        bailout();
}

void assert_str_eq(const char* str1, const char* str2) {
    if (strcmp(str1, str2) != 0)
        bailout();
}

void bailout() {
    fprintf(stderr, "FAIL\n");
    exit(1);
}

void assert_true(int value) {
    if (!value)
        bailout();
}

void assert_false(int value) {
    if (value)
        bailout();
}

void assert_eq(int a, int b) {
    if (a != b)
        bailout();
}

void assert_command_succeed(int ret) {
    if (ret != 0)
        bailout();
}

void assert_command_fails(int ret) {
    if (ret == 0)
        bailout();
}
