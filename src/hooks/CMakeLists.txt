add_library(apprun_hooks SHARED exec.c environment.c interpreter.c exec_args.c)
target_link_libraries(apprun_hooks apprun_common_shared dl)
target_compile_definitions(apprun_hooks PRIVATE "$<$<CONFIG:Debug>:DEBUG>")


install(
    TARGETS apprun_hooks
    LIBRARY DESTINATION lib
)