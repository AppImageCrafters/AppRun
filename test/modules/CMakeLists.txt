add_test(
    NAME TEST_CHECK_GLIBC
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-check-glibc.sh $<TARGET_FILE:check-glibc>
)

add_test(
    NAME TEST_CHECK_GLIBSTDCPP
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-check-glibstdc++.sh $<TARGET_FILE:check-glibstdc++>
)