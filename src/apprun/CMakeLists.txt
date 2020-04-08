add_library(apprun_objects OBJECT ../hooks/environment.c ../hooks/exec_args.c runtime_environmet.c runtime_environmet.h runtime_interpreter.c)
target_link_libraries(apprun_objects -static apprun_common_static)

add_executable(apprun main.c $<TARGET_OBJECTS:apprun_objects>)
target_link_libraries(apprun -static apprun_common_static)

install(
    TARGETS apprun
    RUNTIME DESTINATION bin
)