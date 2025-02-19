cmake_minimum_required(VERSION 3.0)

# PGUPV Library

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_ttf REQUIRED IMPORTED_TARGET SDL2_ttf)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)


file(GLOB SRCS "*.cpp")

IF (WIN32)
MESSAGE("Windows detected.")
SET(GLEW_SRC ${CMAKE_SOURCE_DIR}/librerias/glew/src/glew.c)
SET(GLEW_INC ${CMAKE_SOURCE_DIR}/librerias/glew/include)
ELSE(WIN32)
FIND_PACKAGE(FREEIMAGE)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
else(NOT OPENGL_FOUND)
	message(STATUS ${OpenGL_INCLUDE_DIRS})
	message(STATUS ${OpenGL_LIBRARY_DIRS})
	message(STATUS ${OpenGL_DEFINITIONS})
endif(NOT OPENGL_FOUND)
SET(EXTRA_LIBS  ${OPENGL_LIBRARIES})

ENDIF(WIN32)


add_library(PGUPV ${SRCS} ${GLEW_SRC})

target_include_directories(PGUPV PUBLIC include/ ${CMAKE_SOURCE_DIR}/librerias/glm ${CMAKE_SOURCE_DIR}/librerias/GSL/include PRIVATE ${CMAKE_SOURCE_DIR}/librerias/gli ${CMAKE_SOURCE_DIR}/librerias/json)

target_link_libraries(PGUPV PUBLIC ${EXTRA_LIBS} GLEW::GLEW PkgConfig::SDL2_ttf)
target_link_libraries(PGUPV PRIVATE OpenGL::GL SDL2::SDL2 guipg ${FREEIMAGE_LIBRARIES})



