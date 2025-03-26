cmake_minimum_required(VERSION 2.8)

project(08vectores)

add_executable(08vectores main.cpp gridTools.cpp)
target_link_libraries(08vectores PGUPV vdclib)

set_target_properties( 08vectores PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 08vectores DESTINATION ${CMAKE_SOURCE_DIR}/bin)
