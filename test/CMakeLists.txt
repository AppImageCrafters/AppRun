include_directories(${PROJECT_SOURCE_DIR}/src)

set(APPDIR_MOCK_PATH "${CMAKE_CURRENT_BINARY_DIR}/mock.AppDir")
add_custom_target(
        mock.AppDir
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/create_appdir.sh ${APPDIR_MOCK_PATH} $<TARGET_FILE:AppRun> $<TARGET_FILE:apprun_hooks>
        BYPRODUCTS ${APPDIR_MOCK_PATH}
        DEPENDS AppRun apprun_hooks
)

add_subdirectory(common)
add_subdirectory(hooks)
add_subdirectory(apprun)