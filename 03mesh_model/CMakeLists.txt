cmake_minimum_required(VERSION 3.0)

project(03mesh_model)

add_executable(03mesh_model main.cpp)
target_link_libraries(03mesh_model PGUPV)

set_target_properties( 03mesh_model PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 03mesh_model DESTINATION ${CMAKE_SOURCE_DIR}/bin)
