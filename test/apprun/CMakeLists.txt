add_executable(
        runtime_interpreter_test
        runtime_interpreter_test.c
        ../common/tests_shared.c
        $<TARGET_OBJECTS:apprun_objects>
        $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(runtime_interpreter_test dl)
add_test(NAME TEST_RUNTIME_INTERPRETER COMMAND runtime_interpreter_test)
