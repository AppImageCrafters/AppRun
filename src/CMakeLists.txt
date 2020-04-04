add_library(apprun_exec_hook SHARED exec.c environment.c interpreter.c path.c shared.c)
target_link_libraries(apprun_exec_hook dl)
target_compile_definitions(apprun_exec_hook PRIVATE "$<$<CONFIG:Debug>:DEBUG>")


install(
    TARGETS apprun_exec_hook
    LIBRARY DESTINATION lib
)