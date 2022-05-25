add_executable(check-libc check-libc.c)

add_executable(check-libstdc++ check-libstdc++.cpp)
target_compile_options(check-libstdc++ PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

# check-libstdc++ was written using the least amount of c++ in order to ensure backward compatibility with Centos 6
# and it doesn't get linked to "libstdc++.so.6" therefore we need force the dependency in order to make it available at
# runtime for dl_iterate_phdr
add_custom_command(TARGET check-libstdc++ POST_BUILD
    COMMAND patchelf --add-needed libstdc++.so.6 $<TARGET_FILE:check-libstdc++>
    COMMENT "Adding libstdc++.so.6 as dependency"
    )

install(
    TARGETS check-libc check-libstdc++
    RUNTIME DESTINATION bin
)