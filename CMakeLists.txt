cmake_minimum_required(VERSION 3.0)

project(VDCLAB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (CMAKE_CXX_STANDARD 14)
 







add_subdirectory(librerias/guipg) #Mandatory
add_subdirectory(PGUPV) #Mandatory
add_subdirectory(vdclib) #Mandatory
add_subdirectory(secretFunc) #Mandatory
add_subdirectory(01glm)
add_subdirectory(02pgupv)
add_subdirectory(03mesh_model)
add_subdirectory(04gui)
add_subdirectory(05grids)
add_subdirectory(05vdc)
