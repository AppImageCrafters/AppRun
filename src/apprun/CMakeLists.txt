add_library(
    apprun_objects OBJECT

    ../hooks/environment.c
    ../hooks/exec_args.c
    runtime_environment.c
    runtime_environment.h
    runtime_interpreter.c
)

target_compile_definitions(apprun_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(apprun_objects PUBLIC $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)

add_executable(
    AppRun
    main.c
    $<TARGET_OBJECTS:apprun_objects>
    $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(AppRun -static)
target_compile_definitions(AppRun PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(AppRun PRIVATE $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)
install(
    TARGETS AppRun
    RUNTIME DESTINATION bin
)
add_dependencies(AppRun apprun_hooks)