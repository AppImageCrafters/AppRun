find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)
include_directories(${CHECK_INCLUDE_DIRS})


add_executable(
    file_utils_test
    file_utils_test.c
    tests_shared.c
    $<TARGET_OBJECTS:common_objects>
)

target_compile_definitions(file_utils_test PRIVATE "TESTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/..\"")

add_test(NAME TEST_FILE_UTILS COMMAND file_utils_test)

add_executable(
    path_test
    path_test.c
    tests_shared.c
    $<TARGET_OBJECTS:common_objects>
)
add_test(NAME TEST_PATH COMMAND path_test)

add_executable(
    shell_utils_test
    shell_utils_test.c
    tests_shared.c
    $<TARGET_OBJECTS:common_objects>
)

add_test(NAME TEST_SHELL_UTILS COMMAND shell_utils_test)

add_executable(
    common_test
    common_test.c

    $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(common_test ${CHECK_LIBRARIES} pthread)

add_test(NAME TEST_COMMON COMMAND common_test)

