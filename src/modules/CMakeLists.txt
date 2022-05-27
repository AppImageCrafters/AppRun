add_executable(check-glibc check-glibc.c)
target_link_libraries(check-glibc PRIVATE libconfig common_objects)

add_executable(check-glibstdc++ check-glibstdc++.cpp)
target_compile_options(check-glibstdc++ PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

# check-glibstdc++ was written using the least amount of c++ in order to ensure backward compatibility with Centos 6
# and it doesn't get linked to "libstdc++.so.6" therefore we need force the dependency in order to make it available at
# runtime for dl_iterate_phdr
add_custom_command(TARGET check-glibstdc++ POST_BUILD
    COMMAND patchelf --add-needed libstdc++.so.6 $<TARGET_FILE:check-glibstdc++>
    COMMENT "Adding libstdc++.so.6 as dependency"
    )

install(
    TARGETS check-glibc check-glibstdc++
    RUNTIME DESTINATION bin
)