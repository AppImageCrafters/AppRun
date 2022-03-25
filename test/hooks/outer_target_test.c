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
#include <linux/limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../common/tests_shared.h"

void test_pwd_not_changed() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char cwd[PATH_MAX] = {0x0};
    getcwd(cwd, PATH_MAX);

    assert_str_eq(EXPECTED_WORKDIR, cwd);
    fprintf(stdout, "Ok\n");
}

extern char **environ;

void test_bundle_appdir_env_not_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *appdir_env = getenv("APPDIR");
    assert_true(appdir_env == NULL);

    fprintf(stdout, "Ok\n");
}

void test_bundle_ld_preload_env_not_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("LD_PRELOAD");
    assert_true(env == NULL);

    fprintf(stdout, "Ok\n");
}

void test_bundle_apprun_cwd_env_not_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("APPRUN_CWD");
    assert_true(env == NULL);

    fprintf(stdout, "Ok\n");
}

void test_bundle_apprun_path_mappings_env_not_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("APPDIR_PATH_MAPPINGS_ENV");
    assert_true(env == NULL);

    fprintf(stdout, "Ok\n");
}

int main(int argc, char **argv) {
    system("pwd");
    test_pwd_not_changed();

    test_bundle_appdir_env_not_received();
    test_bundle_ld_preload_env_not_received();
    test_bundle_apprun_cwd_env_not_received();
    test_bundle_apprun_path_mappings_env_not_received();

    return 0;
}

