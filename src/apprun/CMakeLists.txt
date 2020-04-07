add_executable(apprun main.c ../hooks/environment.c ../hooks/exec_args.c runtime_environmet.c runtime_environmet.h)
target_link_libraries(apprun -static apprun_common_static)

install(
    TARGETS apprun
    RUNTIME DESTINATION bin
)