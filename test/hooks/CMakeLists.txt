add_executable(hooks_test hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
add_test(NAME TEST_HOOKS COMMAND hooks_test WORKING_DIRECTORY ..)
set_tests_properties(TEST_HOOKS PROPERTIES ENVIRONMENT LD_PRELOAD=$<TARGET_FILE:apprun_hooks>)
add_dependencies(hooks_test apprun_hooks)

add_executable(environment_test environment_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(environment_test apprun_hooks)
add_test(NAME TEST_ENVIRONMENT_CLEANUP COMMAND environment_test)


string(RANDOM LENGTH 6 MAPPED_APPDIR_PATH_HASH)
set(MAPPED_APPDIR_PATH "/AppDir-${MAPPED_APPDIR_PATH_HASH}")

add_executable(hooks_outer_target_test hooks_outer_target_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        hooks_outer_target_test PRIVATE
        -DEXPECTED_WORKDIR="${CMAKE_CURRENT_BINARY_DIR}"
        -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
)
add_test(NAME TEST_HOOKS_OUTER_TARGET COMMAND hooks_outer_target_test WORKDIR ${CMAKE_CURRENT_BINARY_DIR})

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

# copy resulting binary into the AppDir
add_custom_command(
        TARGET hooks_inner_target_test POST_BUILD
        COMMAND cp $<TARGET_FILE:hooks_inner_target_test> ${APPDIR_MOCK_PATH}
        VERBATIM
)

add_executable(exec_hooks_test exec_hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        exec_hooks_test PRIVATE
        -DAPPDIR_MOCK="${APPDIR_MOCK_PATH}"
        -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
        -DINNER_TARGET="${APPDIR_MOCK_PATH}/$<TARGET_FILE_BASE_NAME:hooks_inner_target_test>"
        -DOUTER_TARGET="${APPDIR_MOCK_PATH}/$<TARGET_FILE_BASE_NAME:hooks_outer_target_test>"
)
add_test(NAME TEST_EXEC_HOOKS COMMAND exec_hooks_test)
set_tests_properties(
        TEST_EXEC_HOOKS PROPERTIES ENVIRONMENT "\
APPDIR=${APPDIR_MOCK_PATH};\
LD_PRELOAD=$<TARGET_FILE:apprun_hooks>;\
APPRUN_CWD=${APPDIR_MOCK_PATH};\
APPRUN_PATH_MAPPINGS=${MAPPED_APPDIR_PATH}:${APPDIR_MOCK_PATH}\;\
APPRUN_STARTUP_APPDIR=${APPDIR_MOCK_PATH};\
APPRUN_STARTUP_LD_PRELOAD=$<TARGET_FILE:apprun_hooks>;\
APPRUN_STARTUP_APPRUN_CWD=${APPDIR_MOCK_PATH};\
APPRUN_STARTUP_APPRUN_PATH_MAPPINGS=${MAPPED_APPDIR_PATH}:${APPDIR_MOCK_PATH}\;\
APPRUN_ORIGINAL_APPDIR=;\
APPRUN_ORIGINAL_LD_PRELOAD=;\
APPRUN_ORIGINAL_APPRUN_CWD=;\
APPRUN_ORIGINAL_APPRUN_PATH_MAPPINGS=;\
"
)

