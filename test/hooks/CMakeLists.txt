add_executable(hooks_test hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        hooks_test PRIVATE
        -DHOOKS_LIB_PATH="$<TARGET_FILE:apprun_hooks>"
)
target_link_libraries(hooks_test PRIVATE apprun_hooks)
add_test(NAME TEST_HOOKS COMMAND hooks_test WORKING_DIRECTORY ..)

add_executable(environment_test environment_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(environment_test apprun_hooks)
add_test(NAME TEST_ENVIRONMENT COMMAND environment_test)


string(RANDOM LENGTH 6 MAPPED_APPDIR_PATH_HASH)
set(MAPPED_APPDIR_PATH "/AppDir-${MAPPED_APPDIR_PATH_HASH}")

# outer target
add_executable(hooks_outer_target_test outer_target_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        hooks_outer_target_test PRIVATE
        -DEXPECTED_WORKDIR="${APPDIR_MOCK_PATH}"
        -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
)
add_dependencies(hooks_outer_target_test mock.AppDir)
add_test(NAME TEST_HOOKS_OUTER_TARGET WORKING_DIRECTORY ${APPDIR_MOCK_PATH} COMMAND hooks_outer_target_test)

# inner target
add_executable(hooks_inner_target_test inner_target_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
target_compile_definitions(
        hooks_inner_target_test PRIVATE
        -DEXPECTED_WORKDIR="${APPDIR_MOCK_PATH}"
        -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
)
add_dependencies(hooks_inner_target_test mock.AppDir)
add_test(NAME TEST_HOOKS_INNER_TARGET COMMAND hooks_inner_target_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(
        TEST_HOOKS_INNER_TARGET PROPERTIES ENVIRONMENT "\
APPDIR=${APPDIR_MOCK_PATH};\
LD_PRELOAD=$<TARGET_FILE:apprun_hooks>;\
APPRUN_CWD=${APPDIR_MOCK_PATH};\
APPRUN_PATH_MAPPINGS=${MAPPED_APPDIR_PATH}:${APPDIR_MOCK_PATH}\;\
"
)

# copy resulting binary into the AppDir and patch linker
add_custom_command(
        TARGET hooks_inner_target_test POST_BUILD
        COMMAND cp $<TARGET_FILE:hooks_inner_target_test> ${APPDIR_MOCK_PATH}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../utils/make_bin_linker_path_relative.sh ${APPDIR_MOCK_PATH}/$<TARGET_FILE_BASE_NAME:hooks_inner_target_test>
        VERBATIM
)

function(configure_exec_hook_test_case Target)
    target_compile_definitions(
            ${Target} PRIVATE
            -DAPPDIR_MOCK="${APPDIR_MOCK_PATH}"
            -DMAPPED_APPDIR_PATH="${MAPPED_APPDIR_PATH}"
            -DHOOKS_LIB_PATH="$<TARGET_FILE:apprun_hooks>"
            -DINNER_TARGET="${APPDIR_MOCK_PATH}/$<TARGET_FILE_BASE_NAME:hooks_inner_target_test>"
            -DOUTER_TARGET="$<TARGET_FILE:hooks_outer_target_test>"
    )
    target_link_libraries(${Target} PRIVATE apprun_hooks)

    string(TOUPPER ${Target} TEST_CASE_NAME)
    string(REPLACE "_TEST" "" TEST_CASE_NAME ${TEST_CASE_NAME})
    add_test(NAME TEST_${TEST_CASE_NAME} WORKING_DIRECTORY ${APPDIR_MOCK_PATH} COMMAND ${Target})
endfunction()

add_executable(execv_hook_on_inner_target_test inner_target_exec_hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
configure_exec_hook_test_case(execv_hook_on_inner_target_test)

add_executable(execv_hook_on_outer_target_test outer_target_exec_hooks_test.c ../common/tests_shared.c $<TARGET_OBJECTS:common_objects>)
configure_exec_hook_test_case(execv_hook_on_outer_target_test)
