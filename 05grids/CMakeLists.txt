cmake_minimum_required(VERSION 2.8)

project(05grids)

add_executable(05grids main.cpp)
target_link_libraries(05grids vdclib PGUPV)

set_target_properties( 05grids PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 05grids DESTINATION ${CMAKE_SOURCE_DIR}/bin)
