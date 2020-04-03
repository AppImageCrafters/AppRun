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

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "tests_shared.h"
#include "../src/shared.h"
#include "../src/interpreter.h"


void test_not_override_if_missing_interpreter_env(char* const* argv) {
    fprintf(stderr, "Test not override on missing INTERPRETER environment variable: ");

    char* filename = strdup(argv[0]);
    appdir_runtime_exec_args_t* new_args = appdir_runtime_adjusted_exec_args(filename, argv);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    appdir_runtime_exec_args_free(new_args);
    free(filename);

    fprintf(stderr, "OK\n");
}

void test_not_override_if_missing_appdir_env(char* const* argv) {
    fprintf(stderr, "Test not override on missing APPDIR environment variable: ");

    char* interpreter = "/lib/ld-linux.so.2";
    setenv("INTERPRETER", interpreter, 1);

    char* filename = strdup(argv[0]);
    appdir_runtime_exec_args_t* new_args = appdir_runtime_adjusted_exec_args(filename, argv);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    appdir_runtime_exec_args_free(new_args);
    free(filename);

    fprintf(stderr, "OK\n");
}

void test_not_override_if_external_binary(char* const* argv) {
    fprintf(stderr, "Test not override when calling external binary: ");

    char* filename = "/usr/bin/echo";
    appdir_runtime_exec_args_t* new_args = appdir_runtime_adjusted_exec_args(filename, argv);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    appdir_runtime_exec_args_free(new_args);

    fprintf(stderr, "OK\n");
}

void test_override_exec_args(char* const* argv) {
    fprintf(stderr, "Test override filename and argv: ");

    char* filename = strdup(argv[0]);
    char* interpreter = "/lib/ld-linux.so.2";
    char* appdir = dirname(filename);

    setenv("INTERPRETER", interpreter, 1);
    setenv("APPDIR", appdir, 1);

    appdir_runtime_exec_args_t* new_args = appdir_runtime_adjusted_exec_args(filename, argv);

    assert_str_eq(new_args->file, interpreter);
    assert_str_eq(new_args->args[0], interpreter);
    assert_str_eq(new_args->args[1], filename);

    char** org_str_list = new_args->args + 1;
    assert_str_list_eq(org_str_list+1, argv+1);

    appdir_runtime_exec_args_free(new_args);
    free(filename);

    fprintf(stderr, "OK\n");
}

void test_appdir_runtime_is_path_child_of() {
    fprintf(stderr, "Test path child of: ");
    assert_false(appdir_runtime_is_path_child_of("/bin/echo", "/usr"));
    assert_true(appdir_runtime_is_path_child_of("/bin/echo", "/bin"));

    assert_false(appdir_runtime_is_path_child_of("/no_existent/echo", "/usr"));
    assert_true(appdir_runtime_is_path_child_of("/no_existent/echo", "/no_existent"));
    fprintf(stderr, "OK\n");
}

void test_appdir_runtime_is_exec_args_change_required() {
    fprintf(stderr, "Test is exec args change required: ");
    assert_false(appdir_runtime_is_exec_args_change_required(NULL, NULL, "/usr/bin/echo"));
    assert_false(appdir_runtime_is_exec_args_change_required(NULL, "/ld.so", "/usr/bin/echo"));
    assert_false(appdir_runtime_is_exec_args_change_required("/usr", NULL, "/usr/bin/echo"));
    assert_true(appdir_runtime_is_exec_args_change_required("/usr", "/ld.so", "/usr/bin/echo"));
    assert_false(appdir_runtime_is_exec_args_change_required("/usr", "/ld.so", "/bin/echo"));
    fprintf(stderr, "OK\n");
}

int main(int argc, char** argv) {
    test_appdir_runtime_is_path_child_of();
    test_appdir_runtime_is_exec_args_change_required();

    test_not_override_if_missing_interpreter_env(argv);
    test_not_override_if_missing_appdir_env(argv);
    test_not_override_if_external_binary(argv);

    test_override_exec_args(argv);

    return 0;
}


