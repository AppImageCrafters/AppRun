#include <check.h>
#include <stdlib.h>

#include "common/string_utils.h"
#include "common/shell_utils.h"

/**
 * String Tests
 * */
START_TEST (test_string_trim_prefix)
    {
        char *result = apprun_string_trim("  a");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST

START_TEST (test_string_trim_postfix)
    {
        char *result = apprun_string_trim("a  ");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST

START_TEST (test_string_trim_both_sides)
    {
        char *result = apprun_string_trim("  a  ");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST

/**
 * Shell Tests
 * */

START_TEST (test_apprun_shell_resolve_var_value_positional_arg)
    {
        char *argv[] = {"zero", "one", "two", NULL};
        char *result = apprun_shell_resolve_var_value(argv, "0");
        ck_assert_str_eq(result, "zero");
        free(result);

        result = apprun_shell_resolve_var_value(argv, "1");
        ck_assert_str_eq(result, "one");
        free(result);

        result = apprun_shell_resolve_var_value(argv, "2");
        ck_assert_str_eq(result, "two");
        free(result);

        result = apprun_shell_resolve_var_value(argv, "@");
        ck_assert_str_eq(result, "\"one\" \"two\"");
        free(result);
    }
END_TEST


START_TEST (test_apprun_shell_resolve_var_value_at_symbol_two_args)
    {
        char *argv[] = {"zero", "one", "two", NULL};
        char *result = apprun_shell_resolve_var_value(argv, "@");
        ck_assert_str_eq(result, "\"one\" \"two\"");
        free(result);
    }
END_TEST

START_TEST (test_apprun_shell_resolve_var_value_at_symbol_zero_args)
    {
        char *argv[] = {"zero", NULL};
        char *result = apprun_shell_resolve_var_value(argv, "@");
        ck_assert_str_eq(result, "");
        free(result);
    }
END_TEST

START_TEST (test_apprun_shell_resolve_var_value_from_env)
    {
        char *argv[] = {"zero", NULL};
        setenv("TEST_VAR", "VALUE", 1);
        char *result = apprun_shell_resolve_var_value(argv, "TEST_VAR");
        ck_assert_str_eq(result, "VALUE");

        unsetenv("TEST_VAR");
        free(result);
    }
END_TEST

START_TEST (test_apprun_shell_resolve_var_value_from_env_missing)
    {
        char *argv[] = {"zero", NULL};
        char *result = apprun_shell_resolve_var_value(argv, "TEST_VAR");
        ck_assert_ptr_eq(result, NULL);
    }
END_TEST

Suite *utils_suite(void) {
    Suite *s;
    TCase *tc_core;
    TCase *tc_shell;

    s = suite_create("Common");

    /* Core test case */
    tc_core = tcase_create("Strings");


    tcase_add_test(tc_core, test_string_trim_prefix);
    tcase_add_test(tc_core, test_string_trim_postfix);
    tcase_add_test(tc_core, test_string_trim_both_sides);
    suite_add_tcase(s, tc_core);

    /* Shell test case */
    tc_shell = tcase_create("Shell");
    tcase_add_test(tc_core, test_apprun_shell_resolve_var_value_positional_arg);
    tcase_add_test(tc_core, test_apprun_shell_resolve_var_value_at_symbol_two_args);
    tcase_add_test(tc_core, test_apprun_shell_resolve_var_value_at_symbol_zero_args);
    tcase_add_test(tc_core, test_apprun_shell_resolve_var_value_from_env);
    tcase_add_test(tc_core, test_apprun_shell_resolve_var_value_from_env_missing);

    suite_add_tcase(s, tc_shell);

    return s;
}


int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = utils_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
