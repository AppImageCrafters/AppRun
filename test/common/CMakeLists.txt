add_executable(file_utils_test file_utils_test.c tests_shared.c)
target_link_libraries(file_utils_test apprun_common_shared)
target_compile_definitions(file_utils_test PRIVATE "TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/..\"")

add_test(NAME TEST_FILE_UTILS COMMAND file_utils_test)

add_executable(path_test path_test.c tests_shared.c)
target_link_libraries(path_test apprun_common_shared)

add_test(NAME TEST_PATH COMMAND path_test)