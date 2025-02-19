cmake_minimum_required(VERSION 2.8)

project(05vdc)

add_executable(05vdc main.cpp buildModel.cpp)
target_link_libraries(05vdc PGUPV vdclib secretFunc)

set_target_properties( 05vdc PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 05vdc DESTINATION ${CMAKE_SOURCE_DIR}/bin)
