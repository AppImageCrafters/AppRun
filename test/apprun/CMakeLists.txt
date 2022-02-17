add_executable(
        runtime_interpreter_test
        runtime_interpreter_test.c
        ../common/tests_shared.c
        $<TARGET_OBJECTS:apprun_objects>
        $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(runtime_interpreter_test dl)
add_test(NAME TEST_RUNTIME_INTERPRETER COMMAND runtime_interpreter_test)


set(APPDIR_MOCK_PATH "${CMAKE_CURRENT_BINARY_DIR}/mock.AppDir")
add_test(
        NAME SETUP_APPDIR_MOCK
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/create_appdir.sh ${APPDIR_MOCK_PATH} $<TARGET_FILE:AppRun> $<TARGET_FILE:apprun_hooks>
)

add_test(NAME TEST_APPRUN_EXEC_AND_CHECK_PWD_CHANGED COMMAND ${APPDIR_MOCK_PATH}/AppRun -c "pwd | grep -vq runtime/compat")
set_tests_properties(TEST_APPRUN_EXEC_AND_CHECK_PWD_CHANGED PROPERTIES DEPENDS SETUP_APPDIR_MOCK)
