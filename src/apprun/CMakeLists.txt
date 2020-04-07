add_executable(AppRun main.c ../hooks/environment.c ../hooks/exec_args.c runtime_environmet.c runtime_environmet.h)
target_link_libraries(AppRun -static apprun_common_static)

install(
    TARGETS AppRun
    RUNTIME DESTINATION bin
)