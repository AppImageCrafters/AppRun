add_library(appimage_exec_wrapper SHARED exec.c environment.c interpreter.c path.c shared.c)
target_link_libraries(appimage_exec_wrapper dl)

install(
    TARGETS appimage_exec_wrapper
    LIBRARY DESTINATION lib
)