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
#include <stdio.h>

#include "common/path.h"
#include "tests_shared.h"

void test_apprun_resolve_file_name() {
    char* result = NULL;
    fprintf(stderr, "Test resolve full path: ");

    result = apprun_resolve_file_name("/bin/bash");
    assert_str_eq("/bin/bash", result);
    fprintf(stderr, "Ok\n");
    free(result);

    fprintf(stderr, "Test resolve relative path: ");
    result = apprun_resolve_file_name("bash");
    assert_str_eq("/bin/bash", result);

    fprintf(stderr, "Ok\n");
    free(result);

}

void test_apprun_is_path_child_of() {
    fprintf(stdout, "Test path child of: ");
    assert_false(apprun_is_path_child_of("/bin/echo", "/usr"));
    assert_true(apprun_is_path_child_of("/bin/echo", "/bin"));

    assert_false(apprun_is_path_child_of("/no_existent/echo", "/usr"));
    assert_true(apprun_is_path_child_of("/no_existent/echo", "/no_existent"));
    fprintf(stdout, "OK\n");
}

int main(int argc, char** argv) {
    test_apprun_is_path_child_of();
    test_apprun_resolve_file_name();
    return 0;
}


