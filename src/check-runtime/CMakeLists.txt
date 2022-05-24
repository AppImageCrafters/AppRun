set(CMAKE_CXX_STANDARD 11)
add_definitions(-std=c++11)

add_executable(check-runtime main.cpp)

install(
    TARGETS check-runtime
    RUNTIME DESTINATION bin
)