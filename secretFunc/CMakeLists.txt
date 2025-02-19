cmake_minimum_required(VERSION 3.0)

project(secretFunc)

add_library(secretFunc main.cpp)

set_target_properties( secretFunc PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

