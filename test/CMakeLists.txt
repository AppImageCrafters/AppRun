add_executable(wrapper_test wrapper_test.c tests_shared.c)
target_link_libraries(wrapper_test appimage_exec_wrapper dl)

add_dependencies(wrapper_test appimage_exec_wrapper)

add_test(NAME TEST_HOOKS
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/wrapper_test_run.sh $<TARGET_FILE:wrapper_test> $<TARGET_FILE:appimage_exec_wrapper>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(interpreter_test interpreter_test.c tests_shared.c)
target_link_libraries(interpreter_test appimage_exec_wrapper dl)
add_test(NAME TEST_INTERPRETER_SETUP COMMAND interpreter_test)

