add_executable(wrapper_test wrapper_test.c ../common/tests_shared.c)
target_link_libraries(wrapper_test apprun_hooks dl)

add_test(NAME TEST_HOOKS
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/wrapper_test_run.sh $<TARGET_FILE:wrapper_test> $<TARGET_FILE:apprun_hooks>
    WORKING_DIRECTORY ..)

add_executable(interpreter_test interpreter_test.c ../common/tests_shared.c)
target_link_libraries(interpreter_test apprun_hooks dl)
add_test(NAME TEST_INTERPRETER_SETUP COMMAND interpreter_test)

add_executable(environment_test environment_test.c ../common/tests_shared.c)
target_link_libraries(environment_test apprun_hooks dl)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)