list(APPEND tests
    test_get_version
    test_hermes
    test_eigenVectorsSerialization
    test_serializerDestructor
)

message(STATUS "Hermes tests...")
foreach(test ${tests})
    message(STATUS "   ...${test}")
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} hermes gtest gtest_main)
    add_test(NAME ${test} COMMAND ${test})
endforeach()

if (${HERMES_ENABLE_PSQL})
  add_subdirectory(postgresql)
endif()
