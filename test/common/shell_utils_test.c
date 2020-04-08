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
#include "common/string_list.h"
#include "common/shell_utils.h"

void test_apprun_shell_expand_variables() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char* res = apprun_shell_expand_variables("$PATH:/bin:${PATH}");
    assert_str_eq(res, "/sbin:/bin:/sbin");
    free(res);

    printf("OK\n");
}

void test_apprun_shell_split_arguments_with_simple_string() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char** res = apprun_shell_split_arguments("hello world");
    char* expected[] = {
            "hello",
            "world",
            NULL
    };
    assert_str_list_eq(res, expected);
    apprun_string_list_free(res);

    printf("OK\n");
}

void test_apprun_shell_split_arguments_with_single_quoted_string() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char** res = apprun_shell_split_arguments("\'hello world\' 1");
    char* expected[] = {"hello world", "1", NULL};
    assert_str_list_eq(res, expected);
    apprun_string_list_free(res);

    printf("OK\n");
}

void test_apprun_shell_split_arguments_with_double_quoted_string() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char** res = apprun_shell_split_arguments("\"hello world\" 2");
    char* expected[] = {"hello world", "2", NULL};
    assert_str_list_eq(res, expected);
    apprun_string_list_free(res);

    printf("OK\n");
}

void test_apprun_shell_split_arguments_with_escapeped_quotes_string() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char** res = apprun_shell_split_arguments("\\\"hello world\\\'");
    char* expected[] = {"\"hello", "world\'", NULL};
    assert_str_list_eq(res, expected);
    apprun_string_list_free(res);

    printf("OK\n");
}

void test_apprun_shell_split_arguments_with_complex_string() {
    printf("%s: ", __PRETTY_FUNCTION__);

    setenv("PATH", "/sbin", 1);
    char** res = apprun_shell_split_arguments("\"\\\'world\'\" \\\\ \\\"hello world\\\' \'\"hello\"\'");
    char* expected[] = {"\\\'world\'", "\\", "\"hello", "world\'", "\"hello\"", NULL};
    assert_str_list_eq(res, expected);
    apprun_string_list_free(res);

    printf("OK\n");
}

int main(int argc, char** argv) {
    test_apprun_shell_expand_variables();
    test_apprun_shell_split_arguments_with_simple_string();
    test_apprun_shell_split_arguments_with_single_quoted_string();
    test_apprun_shell_split_arguments_with_double_quoted_string();
    test_apprun_shell_split_arguments_with_escapeped_quotes_string();
    test_apprun_shell_split_arguments_with_complex_string();
    return 0;
}


