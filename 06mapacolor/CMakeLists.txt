cmake_minimum_required(VERSION 2.8)

project(06mapacolor)

add_executable(06mapacolor main.cpp colormaps.cpp)
target_link_libraries(06mapacolor PGUPV vdclib)

set_target_properties( 06mapacolor PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 06mapacolor DESTINATION ${CMAKE_SOURCE_DIR}/bin)
