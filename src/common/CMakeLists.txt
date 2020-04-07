add_library(apprun_common_static STATIC file_utils.c string_utils.c)
target_include_directories(apprun_common_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(apprun_common_shared SHARED file_utils.c string_utils.c)
target_include_directories(apprun_common_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})