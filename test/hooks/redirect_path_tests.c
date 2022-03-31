#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "hooks/redirect_path.h"
#include "common/appdir_environment.h"

START_TEST (test_redirect_path_full)
    {
        setenv(APPDIR_PATH_MAPPINGS_ENV, "/fake_tmp:/tmp;", 1);
        char *result = apprun_redirect_path("/fake_tmp");
        ck_assert_str_eq(result, "/tmp");
        unsetenv(APPDIR_PATH_MAPPINGS_ENV);
    }
END_TEST

Suite *redirect_path_suite(void) {
    Suite *s;
    TCase *tc_redirect_path;

    s = suite_create("Exec Utils");

    /* Core test case */
    tc_redirect_path = tcase_create("redirect_path_full");
    tcase_add_test(tc_redirect_path, test_redirect_path_full);
    suite_add_tcase(s, tc_redirect_path);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = redirect_path_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
