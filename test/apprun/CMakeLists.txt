add_executable(
        runtime_interpreter_test
        runtime_interpreter_test.c
        ../common/tests_shared.c
        $<TARGET_OBJECTS:apprun_objects>
        $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(runtime_interpreter_test dl)
add_test(NAME TEST_RUNTIME_INTERPRETER COMMAND runtime_interpreter_test)

add_executable(
    libc_utils_test

    libc_utils_test.c
    $<TARGET_OBJECTS:apprun_objects>
    $<TARGET_OBJECTS:common_objects>
)
target_link_libraries(libc_utils_test PRIVATE ${CHECK_LIBRARIES} pthread)
add_test(NAME TEST_LIBC_UTILS COMMAND libc_utils_test)
