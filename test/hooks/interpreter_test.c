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
#include "../../src/hooks/exec_args.h"
#include "../../src/hooks/interpreter.h"
#include "../../src/hooks/path.h"


extern apprun_exec_args_t* apprun_adjusted_exec_args(const char* filename, char* const* argv, char* const* envp);

void test_not_override_if_missing_interpreter_env(char* const* argv) {
    fprintf(stdout, "Test not override on missing INTERPRETER environment variable: ");

    char* filename = strdup(argv[0]);
    apprun_exec_args_t* new_args = apprun_adjusted_exec_args(filename, argv, NULL);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    apprun_exec_args_free(new_args);
    free(filename);

    fprintf(stdout, "OK\n");
}

void test_not_override_if_missing_appdir_env(char* const* argv) {
    fprintf(stdout, "Test not override on missing APPDIR environment variable: ");

    char* interpreter = "/lib/ld-linux.so.2";
    setenv("INTERPRETER", interpreter, 1);

    char* filename = strdup(argv[0]);
    apprun_exec_args_t* new_args = apprun_adjusted_exec_args(filename, argv, NULL);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    apprun_exec_args_free(new_args);
    free(filename);

    fprintf(stdout, "OK\n");
}

void test_not_override_if_external_binary(char* const* argv) {
    fprintf(stdout, "Test not override when calling external binary: ");

    char* filename = "/bin/echo";
    apprun_exec_args_t* new_args = apprun_adjusted_exec_args(filename, argv, NULL);

    assert_str_eq(new_args->file, filename);
    assert_str_list_eq(argv, new_args->args);


    apprun_exec_args_free(new_args);

    fprintf(stdout, "OK\n");
}

void test_override_exec_args(char* const* argv) {
    fprintf(stdout, "Test override filename and argv: ");

    char* filename = strdup(argv[0]);
    char* interpreter = "/lib/ld-linux.so.2";
    char* appdir = dirname(filename);

    setenv("INTERPRETER", interpreter, 1);
    setenv("APPDIR", appdir, 1);

    apprun_exec_args_t* new_args = apprun_adjusted_exec_args(filename, argv, NULL);

    assert_str_eq(new_args->file, interpreter);
    assert_str_eq(new_args->args[0], interpreter);
    assert_str_eq(new_args->args[1], filename);

    char** org_str_list = new_args->args + 1;
    assert_str_list_eq(org_str_list + 1, argv + 1);

    apprun_exec_args_free(new_args);
    free(filename);

    fprintf(stdout, "OK\n");
}

void test_apprun_is_path_child_of() {
    fprintf(stdout, "Test path child of: ");
    assert_false(apprun_is_path_child_of("/bin/echo", "/usr"));
    assert_true(apprun_is_path_child_of("/bin/echo", "/bin"));

    assert_false(apprun_is_path_child_of("/no_existent/echo", "/usr"));
    assert_true(apprun_is_path_child_of("/no_existent/echo", "/no_existent"));
    fprintf(stdout, "OK\n");
}

void test_apprun_is_exec_args_change_required() {
    fprintf(stdout, "Test is apprun args change required: ");
    assert_false(apprun_is_exec_args_change_required(NULL, NULL, "/usr/bin/echo"));
    assert_false(apprun_is_exec_args_change_required(NULL, "/ld.so", "/usr/bin/echo"));
    assert_false(apprun_is_exec_args_change_required("/usr", NULL, "/usr/bin/echo"));
    assert_true(apprun_is_exec_args_change_required("/usr", "/ld.so", "/usr/bin/echo"));
    assert_false(apprun_is_exec_args_change_required("/usr", "/ld.so", "/bin/echo"));
    fprintf(stdout, "OK\n");
}

int main(int argc, char** argv) {
    test_apprun_is_path_child_of();
    test_apprun_is_exec_args_change_required();

    test_not_override_if_missing_interpreter_env(argv);
    test_not_override_if_missing_appdir_env(argv);
    test_not_override_if_external_binary(argv);

    test_override_exec_args(argv);

    return 0;
}


