cmake_minimum_required(VERSION 3.0)

project(01glm)

add_executable(01glm main.cpp)
target_include_directories(01glm PUBLIC ${CMAKE_SOURCE_DIR}/librerias/glm)
set_target_properties( 01glm PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS 01glm DESTINATION ${CMAKE_SOURCE_DIR}/bin)
