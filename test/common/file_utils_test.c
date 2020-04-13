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
#include <common/string_list.h>

#include "tests_shared.h"
#include "common/file_utils.h"
#include "common/path.h"

void test_apprun_file_read_lines() {
    printf("%s: ", __PRETTY_FUNCTION__);

    char** res = apprun_file_read_lines(TESTS_DIR"/apprun/.env");
    assert_true(res != NULL);

    assert_str_eq(res[0], "LD_LOADER=libapprun_hooks-amd64.so");
    assert_str_eq(res[1], "PATH=$APPDIR/bin:$PATH");

    apprun_string_list_free(res);

    printf("Ok\n");
}

void test_resolve_path() {
    char* result = NULL;
    printf("Test resolve full path: ");

    result = apprun_resolve_file_name("/bin/bash");
    assert_str_eq("/bin/bash", result);
    printf("Ok\n");
    free(result);

    printf("Test resolve relative path: ");
    result = apprun_resolve_file_name("bash");
    assert_str_eq("/bin/bash", result);

    printf("Ok\n");
    free(result);
}

int main(int argc, char** argv) {
    test_apprun_file_read_lines();

    test_resolve_path();
    return 0;
}


