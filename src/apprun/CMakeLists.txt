add_library(apprun_objects OBJECT ../hooks/environment.c ../hooks/exec_args.c runtime_environmet.c runtime_environmet.h runtime_interpreter.c)
target_compile_definitions(apprun_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

add_executable(apprun main.c $<TARGET_OBJECTS:apprun_objects>)
target_link_libraries(apprun -static apprun_common_static)
target_compile_definitions(apprun PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

install(
    TARGETS apprun
    RUNTIME DESTINATION bin
)