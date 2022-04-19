add_library(apprun_hooks SHARED exec.c environment.c exec_utils.c constructor.c hooks.c $<TARGET_OBJECTS:common_objects> $<TARGET_OBJECTS:redirect_path_objects> )
target_link_libraries(apprun_hooks PUBLIC dl)
target_include_directories(apprun_hooks PRIVATE $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)
target_compile_definitions(apprun_hooks PRIVATE "$<$<CONFIG:Debug>:DEBUG>")


install(
        TARGETS apprun_hooks
        LIBRARY DESTINATION lib
)