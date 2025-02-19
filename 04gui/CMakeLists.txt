cmake_minimum_required(VERSION 3.0)

project(04gui)

add_executable(04gui main.cpp)
target_link_libraries(04gui PGUPV)

set_target_properties( 04gui PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 04gui DESTINATION ${CMAKE_SOURCE_DIR}/bin)
