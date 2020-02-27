add_library(appimage_exec_wrapper SHARED exec.c)

install(
    TARGETS appimage_exec_wrapper
    LIBRARY DESTINATION lib
)