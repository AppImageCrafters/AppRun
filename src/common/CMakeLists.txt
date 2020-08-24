add_library(
    common_objects OBJECT
    file_utils.c
    string_list.c
    path.c
    string_utils.c
    shell_utils.c
    hook_debug.c
)

target_compile_definitions(common_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(common_objects PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_property(TARGET common_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
