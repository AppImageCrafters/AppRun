add_executable(hooks_test hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
add_test(NAME TEST_HOOKS COMMAND hooks_test WORKING_DIRECTORY ..)
set_tests_properties(TEST_HOOKS PROPERTIES ENVIRONMENT LD_PRELOAD=$<TARGET_FILE:apprun_hooks>)
add_dependencies(hooks_test apprun_hooks)

add_executable(environment_test environment_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(environment_test apprun_hooks)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)


string(RANDOM LENGTH 6 MAPPED_APPDIR_PATH_HASH)
set(MAPPED_APPDIR_PATH "/AppDir-${MAPPED_APPDIR_PATH_HASH}")

add_executable(hooks_inner_target_test hooks_inner_target_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        hooks_inner_target_test PRIVATE
        -DEXPECTED_WORKDIR="${APPDIR_MOCK_PATH}"
        -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
)
add_test(NAME TEST_HOOKS_INNER_TARGET COMMAND hooks_inner_target_test WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(
        TEST_HOOKS_INNER_TARGET PROPERTIES ENVIRONMENT "\
APPDIR=${APPDIR_MOCK_PATH};\
LD_PRELOAD=$<TARGET_FILE:apprun_hooks>;\
APPRUN_CWD=${APPDIR_MOCK_PATH};\
APPRUN_PATH_MAPPINGS=${MAPPED_APPDIR_PATH}:${APPDIR_MOCK_PATH}\;\
"
)
