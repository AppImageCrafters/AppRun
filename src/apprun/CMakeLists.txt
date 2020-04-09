add_library(apprun_objects OBJECT ../hooks/environment.c ../hooks/exec_args.c runtime_environmet.c runtime_environmet.h runtime_interpreter.c)
target_compile_definitions(apprun_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

add_executable(AppRun main.c $<TARGET_OBJECTS:apprun_objects>)
target_link_libraries(AppRun -static apprun_common_static)
target_compile_definitions(AppRun PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

install(
    TARGETS AppRun
    RUNTIME DESTINATION bin
)