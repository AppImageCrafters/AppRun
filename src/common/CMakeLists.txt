set(COMMON_SOURCES file_utils.c string_list.c path.c)

add_library(apprun_common_static STATIC ${COMMON_SOURCES})
target_include_directories(apprun_common_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(apprun_common_shared SHARED ${COMMON_SOURCES})
target_include_directories(apprun_common_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})