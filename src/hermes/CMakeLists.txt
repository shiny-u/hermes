set(hermes_sources
  hermes.h
  field_base.h
  field_base.cpp
  field.h

  visitor.h
  serialization_visitor.h

  serializer.h
  serializer/print_serializer.h
  serializer/print_serializer.cpp
  serializer/csv_serializer.h
  serializer/csv_serializer.cpp

  message.h
  message.cpp

  chrono.h
)
set(hermes_deps
  fmt
  Eigen3::Eigen
  spdlog
)

if (${HERMES_ENABLE_PSQL})
  set(hermes_sources ${hermes_sources}
    serializer/psql_serializer.h
    serializer/psql_serializer.cpp
  )
  set(hermes_deps ${hermes_deps} pqxx)
endif()

if (HERMES_USE_DUNITS)
  set(hermes_deps ${hermes_deps} dunits)
endif ()

add_library(hermes ${hermes_sources})
target_link_libraries(hermes PUBLIC ${hermes_deps})

target_include_directories(hermes PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(hermes PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_compile_definitions(hermes PUBLIC HERMES_MAX_LOG_LEVEL=4)

if (HERMES_USE_DUNITS)
  target_compile_definitions(hermes PUBLIC HAS_DUNITS)
endif ()