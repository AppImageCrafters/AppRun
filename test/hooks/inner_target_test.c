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

void test_pwd_changed() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char cwd[PATH_MAX] = {0x0};
    getcwd(cwd, PATH_MAX);

    assert_str_eq(EXPECTED_WORKDIR, cwd);
    fprintf(stdout, "Ok\n");
}

extern char **environ;

void test_bundle_appdir_env_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *appdir_env = getenv("APPDIR");
    assert_str_eq(appdir_env, EXPECTED_WORKDIR);

    fprintf(stdout, "Ok\n");
}

void test_bundle_ld_preload_env_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("LD_PRELOAD");
    assert_true(env != NULL);

    fprintf(stdout, "Ok\n");
}

void test_bundle_apprun_cwd_env_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("APPRUN_CWD");
    assert_str_eq(env, EXPECTED_WORKDIR);

    fprintf(stdout, "Ok\n");
}

void test_bundle_apprun_path_mappings_env_received() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *env = getenv("APPRUN_PATH_MAPPINGS");
    assert_true(env != NULL);

    fprintf(stdout, "Ok\n");
}

void test_realpath_hook_with_resolved_param() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char result[PATH_MAX];
    char *result_ptr = realpath(MAPPED_APPDIR_PATH, result);
    assert_str_eq(result, EXPECTED_WORKDIR);
    assert_str_eq(result_ptr, EXPECTED_WORKDIR);
    fprintf(stdout, "Ok\n");
}

void test_realpath_hook_without_resolved_param() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *result_ptr = realpath(MAPPED_APPDIR_PATH, NULL);
    assert_str_eq(result_ptr, EXPECTED_WORKDIR);
    free(result_ptr);

    fprintf(stdout, "Ok\n");
}

void test_realpath_hook_on_nonexistent_path() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char result[PATH_MAX] = {0x0};
    char *result_ptr = realpath("/non-existent-file", result);
    assert_str_eq(result, "");
    assert_true(result_ptr == NULL);

    fprintf(stdout, "Ok\n");
}

void test_stat_hook() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char result[PATH_MAX];
    struct stat stat_result;
    int ret = stat(MAPPED_APPDIR_PATH, &stat_result);
    assert_true(ret == 0);

    fprintf(stdout, "Ok\n");
}

void test_fstat_hook() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char result[PATH_MAX];
    int dir_fd = open(MAPPED_APPDIR_PATH, O_DIRECTORY | O_RDONLY);
    struct stat stat_result;
    int ret = fstat(dir_fd, &stat_result);
    assert_true(ret == 0);

    fprintf(stdout, "Ok\n");
}

int main(int argc, char **argv) {
    test_pwd_changed();

    test_bundle_appdir_env_received();
    test_bundle_ld_preload_env_received();
    test_bundle_apprun_cwd_env_received();
    test_bundle_apprun_path_mappings_env_received();

    test_realpath_hook_with_resolved_param();
    test_realpath_hook_without_resolved_param();
    test_realpath_hook_on_nonexistent_path();

    test_stat_hook();
    test_fstat_hook();

    return 0;
}

