#include <check.h>
#include <stdlib.h>

#include "hooks/exec_utils.h"
#include "common/appdir_environment.h"

START_TEST (test_apprun_read_shebang_with_args)
    {
        char *input = "#! /bin/bash  asd\n";
        char *output = apprun_parse_shebang(input, 20);

        ck_assert_ptr_ne(output, NULL);
        ck_assert_str_eq("/bin/bash  asd", output);

        if (output)
            free(output);
    }
END_TEST

START_TEST (test_apprun_read_shebang_without_args)
    {
        char *input = "#!  /bin/bash\n";
        char *output = apprun_parse_shebang(input, 16);

        ck_assert_ptr_ne(output, NULL);
        ck_assert_str_eq("/bin/bash", output);

        if (output)
            free(output);
    }
END_TEST

START_TEST (test_apprun_read_shebang_from_random_bytes)
    {
        char *input = "KAXCJXZCLASKDU";
        char *output = apprun_parse_shebang(input, 14);

        ck_assert_ptr_eq(output, NULL);

        if (output)
            free(output);
    }
END_TEST

START_TEST (test_apprun_shebang_extract_interpreter_path_from_input_with_spaces)
    {
        char *shebang = "/usr/bin/env bash";
        char *output = apprun_shebang_extract_interpreter_path(shebang);
        ck_assert_str_eq(output, "/usr/bin/env");

        free(output);
    }
END_TEST

START_TEST (test_apprun_shebang_extract_interpreter_path_from_input_without_spaces)
    {
        char *shebang = "/usr/bin/env";
        char *output = apprun_shebang_extract_interpreter_path(shebang);
        ck_assert_str_eq(output, "/usr/bin/env");

        free(output);
    }
END_TEST


START_TEST (test_apprun_executable_requires_external_interp)
    {
        char *shebang = "/usr/bin/env";
        bool r = apprun_shebang_requires_external_executable(shebang, "/usr/bin");
        ck_assert_int_eq(r, false);
    }
END_TEST

START_TEST (test_apprun_executable_requires_external_interp_fail)
    {
        char *shebang = "/usr/bin/env";
        bool r = apprun_shebang_requires_external_executable(shebang, "/opt");
        ck_assert_int_eq(r, true);
    }
END_TEST

START_TEST (test_apprun_is_module_path_simple)
    {
        setenv(APPDIR_MODULE_DIR_ENV, "/tmp/one", 1);
        ck_assert(apprun_is_module_path("/tmp/one/a") == true);
        ck_assert(apprun_is_module_path("/tmp/two/a") == false);
        unsetenv(APPDIR_MODULE_DIR_ENV);
    }
END_TEST

START_TEST (test_apprun_is_module_path_multiple)
    {
        setenv(APPDIR_MODULE_DIR_ENV, "/tmp/one:/tmp/two", 1);
        ck_assert(apprun_is_module_path("/tmp/one/a") == true);
        ck_assert(apprun_is_module_path("/tmp/two/a") == true);
        ck_assert(apprun_is_module_path("/tmp/three/a") == false);
        unsetenv(APPDIR_MODULE_DIR_ENV);
    }
END_TEST

START_TEST (test_apprun_is_module_path_empty)
    {
        ck_assert(apprun_is_module_path("/tmp/three/a") == false);
    }
END_TEST

START_TEST (test_resolve_filename_from_path_sh_on_bin)
    {
        char* path_env_orig = strdup(getenv("PATH"));
        setenv("PATH", "/nowhere-to-be-found:/bin:/usr/bin", 1);

        char* result = apprun_resolve_file_from_path_env("sh");
        ck_assert_str_eq(result, "/bin/sh");

        setenv("PATH", path_env_orig, 1);
        free(result);
        free(path_env_orig);
    }
END_TEST

START_TEST (test_resolve_filename_from_path_fail)
    {
        char* result = apprun_resolve_file_from_path_env("this-bin-should-not-be-found");
        ck_assert_str_eq(result, "this-bin-should-not-be-found");
        free(result);
    }
END_TEST

Suite *exec_utils_suite(void) {
    Suite *s;
    TCase *tc_read_shebang;
    TCase *tc_extract_interpreter_path;
    TCase *tc_apprun_executable_requires_external_interp;
    TCase *tc_apprun_modules;
    TCase *tc_resolve_filename_from_path;

    s = suite_create("Exec Utils");

    /* Core test case */
    tc_read_shebang = tcase_create("apprun_parse_shebang");
    tcase_add_test(tc_read_shebang, test_apprun_read_shebang_with_args);
    tcase_add_test(tc_read_shebang, test_apprun_read_shebang_without_args);
    tcase_add_test(tc_read_shebang, test_apprun_read_shebang_from_random_bytes);
    suite_add_tcase(s, tc_read_shebang);

    tc_extract_interpreter_path = tcase_create("apprun_shebang_extract_interpreter_path");
    tcase_add_test(tc_extract_interpreter_path, test_apprun_shebang_extract_interpreter_path_from_input_with_spaces);
    tcase_add_test(tc_extract_interpreter_path, test_apprun_shebang_extract_interpreter_path_from_input_without_spaces);
    suite_add_tcase(s, tc_extract_interpreter_path);

    tc_apprun_executable_requires_external_interp = tcase_create("apprun_shebang_requires_external_executable");
    tcase_add_test(tc_apprun_executable_requires_external_interp, test_apprun_executable_requires_external_interp);
    tcase_add_test(tc_apprun_executable_requires_external_interp, test_apprun_executable_requires_external_interp_fail);
    suite_add_tcase(s, tc_apprun_executable_requires_external_interp);

    tc_apprun_modules = tcase_create("test_apprun_is_module_path");
    tcase_add_test(tc_apprun_modules, test_apprun_is_module_path_simple);
    tcase_add_test(tc_apprun_modules, test_apprun_is_module_path_multiple);
    tcase_add_test(tc_apprun_modules, test_apprun_is_module_path_empty);
    suite_add_tcase(s, tc_apprun_modules);

    tc_resolve_filename_from_path = tcase_create("tc_resolve_filename_from_path");
    tcase_add_test(tc_resolve_filename_from_path, test_resolve_filename_from_path_sh_on_bin);
    tcase_add_test(tc_resolve_filename_from_path, test_resolve_filename_from_path_fail);
    suite_add_tcase(s, tc_resolve_filename_from_path);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = exec_utils_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
