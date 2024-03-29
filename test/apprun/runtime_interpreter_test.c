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

#include <stdio.h>
#include <stdlib.h>
#include <gnu/libc-version.h>

#include "apprun/runtime_interpreter.h"
#include "../common/tests_shared.h"

void test_parse_ld_trace_lib_path() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char *libc_path = parse_ld_trace_line_path("        libc.so.6 => /lib/i386-linux-gnu/libc.so.6 (0xf7c8b000)");
    assert_str_eq(libc_path, "/lib/i386-linux-gnu/libc.so.6");
    free(libc_path);

    char *interpreter_path = parse_ld_trace_line_path("        /lib/ld-linux.so.2 (0xf7f36000)");
    assert_str_eq(interpreter_path, "/lib/ld-linux.so.2");
    free(interpreter_path);

    char *not_found_path = parse_ld_trace_line_path("        libidn.so.11 => not found");
    assert_str_eq(not_found_path, "not found");
    free(not_found_path);

    printf("Ok\n");
}

void test_validate_glibc_version_string() {
    printf("%s: ", __PRETTY_FUNCTION__);
    assert_true(is_linker_version_string_valid("2.3.4"));
    assert_false(is_linker_version_string_valid("PRIVATE"));

    printf("Ok\n");
}

void test_read_ld_version() {
    printf("%s: ", __PRETTY_FUNCTION__);
    char *version = read_ld_version("/lib64/ld-linux-x86-64.so.2");
    const char *expected_version = gnu_get_libc_version();
    assert_str_eq(version, expected_version);
    free(version);

    printf("Ok\n");
}

void test_compare_glib_version_strings() {
    printf("%s: ", __PRETTY_FUNCTION__);

    assert_eq(compare_version_strings("1", "1"), 0);
    assert_eq(compare_version_strings("2", "1"), 1);
    assert_eq(compare_version_strings("1", "2"), -1);
    assert_eq(compare_version_strings("1.1", "1.1"), 0);
    assert_eq(compare_version_strings("1.2", "1.1"), 1);
    assert_eq(compare_version_strings("1.1", "1.2"), -1);
    assert_eq(compare_version_strings("1.2", "1.2.1"), -1);
    assert_eq(compare_version_strings("1.2.1", "1.2.1"), 0);
    assert_eq(compare_version_strings("1.2.1", "1.2"), 1);

    assert_true(compare_version_strings(NULL, "1") < 0);
    assert_true(compare_version_strings("1", NULL) > 0);
    assert_true(compare_version_strings(NULL, NULL) == 0);

    printf("Ok\n");
}

int main(int argc, char **argv, char *envp[]) {
    test_parse_ld_trace_lib_path();
    test_validate_glibc_version_string();
    test_compare_glib_version_strings();
    test_read_ld_version();

    return 0;
}



