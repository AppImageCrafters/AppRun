#include <check.h>
#include <stdlib.h>

#include "common/string_utils.h"

START_TEST (test_string_trim_prefix)
    {
        char* result = apprun_string_trim("  a");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST

START_TEST (test_string_trim_postfix)
    {
        char* result = apprun_string_trim("a  ");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST

START_TEST (test_string_trim_both_sides)
    {
        char* result = apprun_string_trim("  a  ");
        ck_assert_str_eq(result, "a");
        free(result);
    }
END_TEST


Suite* utils_suite(void) {
    Suite* s;
    TCase* tc_core;
    TCase* tc_limits;

    s = suite_create("Utils");

    /* Core test case */
    tc_core = tcase_create("Strings");


    tcase_add_test(tc_core, test_string_trim_prefix);
    tcase_add_test(tc_core, test_string_trim_postfix);
    tcase_add_test(tc_core, test_string_trim_both_sides);
    suite_add_tcase(s, tc_core);

    return s;
}


int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = utils_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
