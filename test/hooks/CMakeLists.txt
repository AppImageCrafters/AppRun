add_executable(wrapper_test wrapper_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)

add_test(NAME TEST_HOOKS
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/wrapper_test_run.sh $<TARGET_FILE:wrapper_test> $<TARGET_FILE:apprun_hooks>
    WORKING_DIRECTORY ..)

add_executable(environment_test environment_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(environment_test apprun_hooks)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)