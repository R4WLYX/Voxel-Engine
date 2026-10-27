#pragma once

#include "window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Engine {
  public:
    Engine(const WindowContext&);
    ~Engine() = default;

    Window& window();

  private:
    struct GLFW {
        GLFW() { glfwInit(); }
        ~GLFW() { glfwTerminate(); }
    } _glfw;
    
    Window _window;

    static void resize_callback(GLFWwindow*, int, int);
};

inline Engine::Engine(const WindowContext& ctx) : _window(ctx) {
    _window.make_current();

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Error: Failed to initialize OpenGL!");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    _window.set_resize_callback(resize_callback);
}

inline Window& Engine::window() { return _window; }

inline void Engine::resize_callback(GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); }