cmake_minimum_required(VERSION 2.8)

project(07contornos)

add_executable(07contornos main.cpp contornos.cpp)
target_link_libraries(07contornos PGUPV vdclib)

set_target_properties( 07contornos PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 07contornos DESTINATION ${CMAKE_SOURCE_DIR}/bin)
