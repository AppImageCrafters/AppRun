add_library(
        common_objects OBJECT
        file_utils.c
        string_list.c
        path.c
        string_utils.c
        shell_utils.c
        hook_debug.c
        appdir_environment.h
)

target_compile_definitions(common_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(common_objects PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_property(TARGET common_objects PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(
    redirect_path_objects OBJECT
    redirect_path.c
)
target_compile_definitions(redirect_path_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(redirect_path_objects PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
set_property(TARGET redirect_path_objects PROPERTY POSITION_INDEPENDENT_CODE ON)