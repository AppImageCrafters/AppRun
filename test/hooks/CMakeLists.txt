add_executable(hooks_test hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
add_test(NAME TEST_HOOKS COMMAND hooks_test WORKING_DIRECTORY ..)
set_tests_properties(TEST_HOOKS PROPERTIES ENVIRONMENT LD_PRELOAD=$<TARGET_FILE:apprun_hooks>)
add_dependencies(hooks_test apprun_hooks)

add_executable(environment_test environment_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(environment_test apprun_hooks)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)