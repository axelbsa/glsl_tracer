cmake_minimum_required(VERSION 3.18)

project(main LANGUAGES CXX)

find_package(glfw3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
add_compile_definitions(GL_SILENCE_DEPRECATION)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)
message(STATUS "Found GLEW in ${GLEW_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1y -Wall -Wextra -Wunused-function")

add_executable(main src/main.cpp
        src/window.cpp
        src/window.h
        src/shader.h
        src/camera.h
        src/Sphere.h)
target_link_libraries(main glfw GLEW)
target_link_libraries(main OpenGL::GL)
