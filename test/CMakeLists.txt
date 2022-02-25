include_directories(${PROJECT_SOURCE_DIR}/src)

set(APPDIR_MOCK_PATH "${CMAKE_CURRENT_BINARY_DIR}/mock.AppDir")


add_subdirectory(common)
add_subdirectory(hooks)
add_subdirectory(apprun)

add_custom_target(
        mock.AppDir
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/create_appdir.sh ${APPDIR_MOCK_PATH} ${PROJECT_BINARY_DIR}
        BYPRODUCTS ${APPDIR_MOCK_PATH}/AppRun
        DEPENDS AppRun apprun_hooks hooks_inner_target_test
)

# remove mock on make clean
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${APPDIR_MOCK_PATH} )