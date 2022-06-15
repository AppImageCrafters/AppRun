include_directories(${PROJECT_SOURCE_DIR}/src)

set(APPDIR_MOCK_PATH "${CMAKE_CURRENT_BINARY_DIR}/mock.AppDir")

string(RANDOM LENGTH 6 MAPPED_APPDIR_PATH_HASH)
set(MAPPED_APPDIR_PATH "/AppDir-${MAPPED_APPDIR_PATH_HASH}")

add_subdirectory(common)
add_subdirectory(hooks)

add_custom_target(
    mock.AppDir
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/create_appdir.sh ${APPDIR_MOCK_PATH} ${MAPPED_APPDIR_PATH} ${PROJECT_BINARY_DIR}
    BYPRODUCTS ${APPDIR_MOCK_PATH}/AppRun
    DEPENDS AppRun apprun_hooks hooks_inner_target_test check-glibc check-glibstdc++
)

# remove mock on make clean
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${APPDIR_MOCK_PATH})

add_subdirectory(modules)