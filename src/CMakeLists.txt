add_library(appimage_exec_wrapper SHARED exec.c)
set(CMAKE_C_FLAGS "-Wl,--no-as-needed")

install(
    TARGETS appimage_exec_wrapper
    LIBRARY DESTINATION lib
)