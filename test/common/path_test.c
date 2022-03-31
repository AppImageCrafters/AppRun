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
#include <string.h>
#include <linux/limits.h>

#include "common/path.h"
#include "common/appdir_environment.h"
#include "tests_shared.h"


void test_apprun_resolve_absolute_bin_path() {
    char *result = NULL;
    fprintf(stderr, "%s: ", __FUNCTION__);

    char *orig_path = strdup(getenv("PATH"));
    setenv("PATH", APPDIR_MOCK_PATH"/usr/bin", 1);

    result = apprun_resolve_bin_path(APPDIR_MOCK_PATH"/bin/app");
    assert_str_eq(APPDIR_MOCK_PATH"/usr/bin/app", result);

    fprintf(stderr, "Ok\n");

    setenv("PATH", orig_path, 1);
    free(result);
    free(orig_path);
}

void test_apprun_resolve_bin_filename() {
    char *result = NULL;
    fprintf(stderr, "%s: ", __FUNCTION__);

    char *orig_path = strdup(getenv("PATH"));
    setenv("PATH", APPDIR_MOCK_PATH"/usr/bin", 1);
    result = apprun_resolve_bin_path("app");
    assert_str_eq(APPDIR_MOCK_PATH"/usr/bin/app", result);

    setenv("PATH", orig_path, 1);
    fprintf(stderr, "Ok\n");
    free(result);
    free(orig_path);
}

void test_apprun_is_path_child_of_on_existent_paths() {
    fprintf(stdout, "%s: ", __FUNCTION__);
    assert_false(apprun_is_path_child_of(APPDIR_MOCK_PATH"/usr/", APPDIR_MOCK_PATH"/usr/bin/"));
    assert_true(apprun_is_path_child_of(APPDIR_MOCK_PATH"/usr/bin/app", APPDIR_MOCK_PATH));
    fprintf(stdout, "OK\n");
}

void test_apprun_is_path_child_of_on_non_existent_paths() {
    fprintf(stdout, "%s: ", __FUNCTION__);
    assert_false(apprun_is_path_child_of("/no_existent/echo", "/usr"));
    assert_true(apprun_is_path_child_of("/no_existent/echo", "/no_existent"));
    fprintf(stdout, "OK\n");
}

void test_apprun_is_path_child_of_on_mapped_paths() {
    fprintf(stdout, "%s: ", __FUNCTION__);
    set_private_env(APPDIR_PATH_MAPPINGS_ENV, "/mapped.AppDir:"APPDIR_MOCK_PATH";");

    assert_false(apprun_is_path_child_of("/mapped.AppDir/usr/", APPDIR_MOCK_PATH"/usr/bin/"));
    assert_true(apprun_is_path_child_of("/mapped.AppDir/usr/bin/app", APPDIR_MOCK_PATH));

    unset_private_env(APPDIR_PATH_MAPPINGS_ENV);
    fprintf(stdout, "OK\n");
}

void test_apprun_concat_path() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char res[PATH_MAX] = {0x0};
    apprun_concat_path(res, "/");
    assert_str_eq(res, "/");

    apprun_concat_path(res, "/AppDir");
    assert_str_eq(res, "/AppDir");

    apprun_concat_path(res, "");
    assert_str_eq(res, "/AppDir");

    apprun_concat_path(res, "usr/");
    assert_str_eq(res, "/AppDir/usr/");

    apprun_concat_path(res, "/bin/");
    assert_str_eq(res, "/AppDir/usr/bin/");

    fprintf(stdout, "OK\n");
}

int main(int argc, char **argv) {
    test_apprun_resolve_absolute_bin_path();
    test_apprun_resolve_bin_filename();

    test_apprun_is_path_child_of_on_existent_paths();
    test_apprun_is_path_child_of_on_non_existent_paths();
    test_apprun_is_path_child_of_on_mapped_paths();

    test_apprun_concat_path();
    return 0;
}


