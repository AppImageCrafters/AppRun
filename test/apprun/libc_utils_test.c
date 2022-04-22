/**************************************************************************
 *
 * Copyright (c) 2022 Alexis Lopez Zubieta <contact@azubieta.net>
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

#include <check.h>
#include <stdlib.h>

#include <apprun/libc_utils.h>
#include <regex.h>


START_TEST (test_read_libc_path)
    {
        char* result = apprun_read_glibc_path_from_so_cache();
        ck_assert_ptr_ne(result, NULL);

        free(result);
    }
END_TEST

START_TEST (test_libc_version_string_regex)
    {
        regex_t regex;
        int regcomp_res = regcomp(&regex, GLIBC_VERSION_STRING_REGEX, REG_EXTENDED);
        ck_assert_int_eq(regcomp_res, 0);

        int regexec_res = regexec(&regex, "glibc 2.31", 0, NULL, 0);
        ck_assert_int_eq(regexec_res, 0);
    }
END_TEST

START_TEST (test_read_libc_version)
    {
        char* glibc_path = apprun_read_glibc_path_from_so_cache();
        ck_assert_ptr_ne(glibc_path, NULL);

        char* glibc_version = apprun_read_glibc_version_from_lib(glibc_path);
        ck_assert_ptr_ne(glibc_version, NULL);

        free(glibc_version);
        free(glibc_path);
    }
END_TEST

Suite* libc_utils_suite(void) {
    Suite* s;
    TCase* tc_read_libc_path;
    TCase* tc_read_libc_version;

    s = suite_create("LibC Utils");

    tc_read_libc_path = tcase_create("tc_read_libc_path");
    tcase_add_test(tc_read_libc_path, test_read_libc_path);
    suite_add_tcase(s, tc_read_libc_path);

    tc_read_libc_version = tcase_create("tc_read_libc_version");
    tcase_add_test(tc_read_libc_version, test_libc_version_string_regex);
    tcase_add_test(tc_read_libc_version, test_read_libc_version);
    suite_add_tcase(s, tc_read_libc_version);

    return s;
}

int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = libc_utils_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
