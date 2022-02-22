add_library(apprun_hooks SHARED exec.c environment.c exec_utils.c main_hook.c hooks.c redirect_path.c $<TARGET_OBJECTS:common_objects>)
target_link_libraries(apprun_hooks dl)
target_include_directories(apprun_hooks PRIVATE $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)
target_compile_definitions(apprun_hooks PRIVATE "$<$<CONFIG:Debug>:DEBUG>")


install(
    TARGETS apprun_hooks
    LIBRARY DESTINATION lib
)