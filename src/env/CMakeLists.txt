add_library(
    env_objects OBJECT

    ../hooks/environment.c
    ../hooks/exec_args.c
    runtime_environment.c
    runtime_environment.h
    runtime_interpreter.c
)

target_compile_definitions(env_objects PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(env_objects PUBLIC $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)

add_executable(
    env
    main.c
    $<TARGET_OBJECTS:env_objects>
    $<TARGET_OBJECTS:common_objects>
)

target_link_libraries(env -static)
target_compile_definitions(env PRIVATE "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(env PRIVATE $<TARGET_PROPERTY:common_objects,INCLUDE_DIRECTORIES>)
install(
    TARGETS env
    RUNTIME DESTINATION bin
)