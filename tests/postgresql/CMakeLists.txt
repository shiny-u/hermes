set(test test_postgresql)

message(STATUS "   ...${test}")
add_executable(${test} "${test}.cpp")
target_link_libraries(${test} hermes gtest gtest_main)
add_test(NAME ${test} COMMAND ${test})
