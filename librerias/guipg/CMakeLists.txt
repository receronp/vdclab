find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)


find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library (guipg 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui_draw.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui_widgets.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/examples/imgui_impl_sdl.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/examples/imgui_impl_opengl3.cpp
    nfd_common.c 
    nfd_gtk.c
    guipg.cpp
    icons.cpp
    imGuIZMOquat.cpp
    logConsole.cpp
    )

target_compile_definitions(guipg PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW GLM_ENABLE_EXPERIMENTAL IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUIZMO_IMGUI_FOLDER= GLEW_USE_STATIC_LIBS)
target_compile_options(guipg PRIVATE ${GTK3_CFLAGS_OTHER})

target_include_directories(guipg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(guipg PRIVATE 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/examples/ 
    ${GTK3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/librerias/glm
    ${CMAKE_SOURCE_DIR}/librerias/GSL/include
    )

#MESSAGE("${GTK3_LIBRARY_DIRS}")
#target_link_directories(guipg PUBLIC ${GTK3_LIBRARY_DIRS})

#INCLUDE_DIRECTORIES($)

#link_directories(${GTK3_LIBRARY_DIRS})

target_link_libraries(guipg PRIVATE OpenGL::GL SDL2::SDL2 GLEW::GLEW PkgConfig::GTK3) # ${GTK3_LIBRARIES})
