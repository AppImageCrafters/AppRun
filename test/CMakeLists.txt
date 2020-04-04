add_executable(wrapper_test wrapper_test.c tests_shared.c)
target_link_libraries(wrapper_test apprun_hooks dl)

add_test(NAME TEST_HOOKS
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/wrapper_test_run.sh $<TARGET_FILE:wrapper_test> $<TARGET_FILE:apprun_hooks>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(interpreter_test interpreter_test.c tests_shared.c)
target_link_libraries(interpreter_test apprun_hooks dl)
add_test(NAME TEST_INTERPRETER_SETUP COMMAND interpreter_test)

add_executable(path_test path_test.c tests_shared.c)
target_link_libraries(path_test apprun_hooks dl)
add_test(NAME TEST_FILE_NAME_RESOLUTION COMMAND path_test)

add_executable(environment_test environment_test.c tests_shared.c)
target_link_libraries(environment_test apprun_hooks dl)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)