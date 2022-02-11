add_executable(hello_world_app hello_world_app.c)
set(INTERP_FILE_NAME /appimage-XXXXXXX-ld-linux.so)
add_custom_command(
    TARGET hello_world_app POST_BUILD
    COMMAND patchelf --set-interpreter "/dev/shm/${INTERP_FILE_NAME}" hello_world_app
    VERBATIM
)

set(PATCHED_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/hello_world_app)

add_executable(
    runtime_interpreter_test
    runtime_interpreter_test.c
    ../common/tests_shared.c
    $<TARGET_OBJECTS:apprun_objects>
    $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(runtime_interpreter_test apprun_hooks dl rt)
target_compile_definitions(runtime_interpreter_test PRIVATE "PATCHED_BIN_PATH=\"${PATCHED_BIN_PATH}\"")
target_compile_definitions(runtime_interpreter_test PRIVATE "INTERP_FILE_NAME=\"${INTERP_FILE_NAME}\"")
add_test(NAME TEST_RUNTIME_INTERPRETER COMMAND runtime_interpreter_test)
add_dependencies(runtime_interpreter_test hello_world_app)
