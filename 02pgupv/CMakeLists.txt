cmake_minimum_required(VERSION 3.0)

project(02pgupv)

add_executable(02pgupv main.cpp)
target_link_libraries(02pgupv PGUPV)

set_target_properties( 02pgupv PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 02pgupv DESTINATION ${CMAKE_SOURCE_DIR}/bin)
