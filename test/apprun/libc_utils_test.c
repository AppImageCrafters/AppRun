#include <check.h>
#include <stdlib.h>

#include <apprun/libc_utils.h>


START_TEST (test_read_libc_path)
    {
        char* result = apprun_read_libc_path_from_so_cache();
        ck_assert_ptr_ne(result, NULL);

        free(result);
    }
END_TEST

Suite* libc_utils_suite(void) {
    Suite* s;
    TCase* tc_read_libc_path;


    s = suite_create("LibC Utils");

    tc_read_libc_path = tcase_create("tc_read_libc_path");
    tcase_add_test(tc_read_libc_path, test_read_libc_path);
    suite_add_tcase(s, tc_read_libc_path);

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
