include_directories(${PROJECT_SOURCE_DIR}/src)

set(APPDIR_MOCK_PATH "${CMAKE_CURRENT_BINARY_DIR}/mock.AppDir")
add_test(
        NAME SETUP_APPDIR_MOCK
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/create_appdir.sh ${APPDIR_MOCK_PATH} $<TARGET_FILE:AppRun> $<TARGET_FILE:apprun_hooks>
)
set_tests_properties(SETUP_APPDIR_MOCK PROPERTIES DEPENDS AppRun)
set_tests_properties(SETUP_APPDIR_MOCK PROPERTIES DEPENDS apprun_hooks)

add_subdirectory(common)
add_subdirectory(hooks)
add_subdirectory(apprun)