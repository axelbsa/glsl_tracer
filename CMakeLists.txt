cmake_minimum_required(VERSION 3.18)

project(main LANGUAGES C CXX)

if(WIN32)
    set(OpenGL_GL_PREFERENCE "LEGACY")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)

    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(GLAD_DIR "${CMAKE_SOURCE_DIR}/src")
    include_directories(${CMAKE_SOURCE_DIR}\\src\\glad)
    include_directories(${CMAKE_SOURCE_DIR}\\src\\KHR)

    find_package(OpenGL REQUIRED)

    add_executable(main
        src/glad.c
        src/glad/glad.h
        src/main.cpp
        src/window.cpp
        src/window.h
        src/shader.h
        src/camera.h
            src/sphere.h
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    message(STATUS "GLAD is pointing to ${GLAD_DIR}")
    target_include_directories(main SYSTEM PRIVATE src)

    #target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_DIR}")
    #target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE $<BUILD_INTERFACE:${GLAD_DIR}>)
    #target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

    target_link_libraries(main glfw)
    target_link_libraries(main OpenGL::GL)

else()

    find_package(glfw3 REQUIRED)
    message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    # target_include_directories(main SYSTEM PRIVATE src)


    set(OpenGL_GL_PREFERENCE "LEGACY")
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1y -Wall -Wextra -Wunused-function")

    add_executable(main
            src/glad.c
            src/glad/glad.h
            src/main.cpp
            src/window.cpp
            src/window.h
            src/shader.h
            src/camera.h
            src/sphere.h)

    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    if (APPLE)
        target_link_libraries(main glfw OpenGL::GL)
    else ()
        find_package(GLEW REQUIRED)
        message(STATUS "Found GLEW in ${GLEW_LIBRARIES}")
        target_link_libraries(main glfw GLEW)
        target_link_libraries(main OpenGL::GL)
    endif ()
endif()
