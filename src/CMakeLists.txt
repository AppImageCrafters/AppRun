add_library(appimage_exec_wrapper SHARED exec.c environment.c interpreter.c path.c shared.c)
target_link_libraries(appimage_exec_wrapper dl)
target_compile_definitions(appimage_exec_wrapper PRIVATE "$<$<CONFIG:Debug>:DEBUG>")


install(
    TARGETS appimage_exec_wrapper
    LIBRARY DESTINATION lib
)