add_executable(
        runtime_interpreter_test
        runtime_interpreter_test.c
        ../common/tests_shared.c
        $<TARGET_OBJECTS:apprun_objects>
        $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(runtime_interpreter_test dl)
add_test(NAME TEST_RUNTIME_INTERPRETER COMMAND runtime_interpreter_test)

add_test(NAME TEST_APPRUN_EXEC_AND_CHECK_PWD_CHANGED COMMAND ${APPDIR_MOCK_PATH}/AppRun -c "$APPDIR/usr/bin/pwd | grep -vq runtime/compat")
set_tests_properties(TEST_APPRUN_EXEC_AND_CHECK_PWD_CHANGED PROPERTIES DEPENDS SETUP_APPDIR_MOCK)

add_test(NAME TEST_APPRUN_EXEC_AND_CHECK_PWD_NOT_CHANGED COMMAND ${APPDIR_MOCK_PATH}/AppRun -c "pwd | grep -vq runtime/compat")
set_tests_properties(TEST_APPRUN_EXEC_AND_CHECK_PWD_NOT_CHANGED PROPERTIES DEPENDS SETUP_APPDIR_MOCK)