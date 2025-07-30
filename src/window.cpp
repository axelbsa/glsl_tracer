//
// Created by Axel Sanner on 21/11/2023.
//
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#elif __linux__
#include "glad/glad.h"
#else
#include "glad/glad.h"
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "limits.h"

#include "window.h"

#include <string.h>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    // Update your stored framebuffer dimensions
}

static void ErrorCallback(int, const char* err_str)
{
    std::cerr << "GLFW Error: " << err_str << std::endl;
}

uint32_t window::init() {
    glfwSetErrorCallback(ErrorCallback);
    if(glfwInit() == GL_FALSE) {
        std::cerr << "failed to init GLFW" << std::endl;
        return 1;
    }
    return 0;
}

uint32_t window::create() {
    if((w = glfwCreateWindow(width, height, "", 0, 0)) == 0) {
        std::cerr << "failed to open window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    glfwSwapInterval( 0 );

    glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);

#ifndef __APPLE__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD");
        return -1;
    }
#endif

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(w, &fbWidth, &fbHeight);

    // Update your class members or store these values
    framebuffer_width = fbWidth;
    framebuffer_height = fbHeight;

    // Set viewport to framebuffer size
    glViewport(0, 0, fbWidth, fbHeight);
    checkSSBOSupport();
    queryOpenGLLimits();

    return 0;
}

uint32_t window::setWindowHints(uint32_t hint, uint32_t value) {
    // Check return and act on it
    glfwWindowHint(hint, value);
    return 0;
}

GLFWwindow *window::getGLFWWindow() {
    return w;
}

void window::queryOpenGLLimits() {
    GLint value;

    std::cout << "=== OpenGL Buffer Limits ===" << std::endl;

    fprintf(stderr, "FOOOOOOO\n");

    // Maximum uniform block size (UBO)
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &value);
    std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE: " << value << " bytes ("
              << static_cast<float>(value) / 1024.0f << " KB)" << std::endl;

    fprintf(stderr, "BARRRRRRRRR\n");
    // Maximum number of uniform buffer bindings
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);
    std::cout << "GL_MAX_UNIFORM_BUFFER_BINDINGS: " << value << std::endl;

    // Maximum combined uniform blocks (vertex + fragment + geometry)
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &value);
    std::cout << "GL_MAX_COMBINED_UNIFORM_BLOCKS: " << value << std::endl;

    // Per-stage uniform blocks
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &value);
    std::cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS: " << value << std::endl;

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &value);
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_BLOCKS: " << value << std::endl;

    // Traditional uniform limits (for comparison)
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &value);
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: " << value
              << " (" << value * 4 << " bytes)" << std::endl;

    // Texture buffer limits (useful alternative to SSBOs on 4.1)
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &value);
    std::cout << "GL_MAX_TEXTURE_BUFFER_SIZE: " << value << " texels" << std::endl;

    // Maximum texture size (another storage option)
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
    std::cout << "GL_MAX_TEXTURE_SIZE: " << value << "x" << value << std::endl;

    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &value);
    std::cout << "GL_MAX_3D_TEXTURE_SIZE: " << value << "^3" << std::endl;

#ifndef __APPLE__
    // Check if SSBO is supported (will be 0 on OpenGL 4.1)
    GLint ssboSupported = 0;
    // This query will fail gracefully on OpenGL < 4.3
    if (glGetError() == GL_NO_ERROR) {
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &ssboSupported);
        if (glGetError() == GL_NO_ERROR && ssboSupported > 0) {
            std::cout << "SSBO Support: YES" << std::endl;
            glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &value);
            std::cout << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE: " << value
                      << " bytes (" << value / (1024.0f * 1024.0f) << " MB)" << std::endl;
        } else {
            std::cout << "SSBO Support: NO (OpenGL 4.3+ required)" << std::endl;
        }
    } else {
        std::cout << "SSBO Support: NO (OpenGL 4.3+ required)" << std::endl;
        glGetError(); // Clear error
    }
#endif

    // OpenGL version info
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    std::cout << "\nOpenGL Version: " << version << std::endl;
    std::cout << "Renderer: " << renderer << std::endl;

    // Check for any OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << std::endl;
    }
}

bool window::checkSSBOSupport() {
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if (major > 4 || (major == 4 && minor >= 3)) {
        // Is guaranteed in 4.3 and above
        return true;
    }

    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    for (int i = 0; i < numExtensions; i++) {
        const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
        if (strcmp(ext, "GL_ARB_shader_storage_buffer_object") == 0) {
            std::cout << "SSBO SUPPORTED" << std::endl;
            return true;
        }else {
            std::cout << ext << std::endl;
        }
    }
    return false;
}
