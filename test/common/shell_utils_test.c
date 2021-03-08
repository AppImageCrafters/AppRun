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
    char* res = apprun_shell_expand_variables("$PATH:/bin:${PATH}", NULL);
    assert_str_eq(res, "/sbin:/bin:/sbin");
    free(res);

    printf("OK\n");
}

void test_apprun_shell_expand_command_line_arguments() {
    printf("%s: ", __PRETTY_FUNCTION__);
    char* argv[] = {"HELLO", "EXTENDED WORLD", "AGAIN", NULL};
    setenv("PATH", "/sbin", 1);
    char* res = apprun_shell_expand_variables("$0:$@", argv);
    assert_str_eq(res, "HELLO:\"EXTENDED WORLD\" \"AGAIN\"");
    free(res);

    printf("OK\n");
}


void test_apprun_shell_split_arguments_single_arg() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash");
    char* expected[] = {"/bin/bash", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls $PWD");
    char* expected[] = {"/bin/bash", "ls", "$PWD", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args_including_double_quotes() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls /this\"/spaced path\"/here");
    char* expected[] = {"/bin/bash", "ls", "/this/spaced path/here", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args_including_single_quotes() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls /this\'/spaced path\'/here");
    char* expected[] = {"/bin/bash", "ls", "/this/spaced path/here", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args_including_mixed_quotes() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls '\"echo\"'");
    char* expected[] = {"/bin/bash", "ls", "\"echo\"", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args_including_mixed_quotes_inverse() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls \"'echo'\"");
    char* expected[] = {"/bin/bash", "ls", "'echo'", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

void test_apprun_shell_split_arguments_many_args_including_scape_char() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_shell_split_arguments("/bin/bash ls \\ dir");
    char* expected[] = {"/bin/bash", "ls", " dir", 0};
    assert_str_list_eq(res, expected);
    free(res);

    printf("Ok\n");
}

int main(int argc, char** argv) {
    test_apprun_shell_expand_variables();
    test_apprun_shell_expand_command_line_arguments();
    test_apprun_shell_split_arguments_single_arg();
    test_apprun_shell_split_arguments_many_args();
    test_apprun_shell_split_arguments_many_args_including_double_quotes();
    test_apprun_shell_split_arguments_many_args_including_single_quotes();
    test_apprun_shell_split_arguments_many_args_including_mixed_quotes();
    test_apprun_shell_split_arguments_many_args_including_mixed_quotes_inverse();
    test_apprun_shell_split_arguments_many_args_including_scape_char();
    return 0;
}


