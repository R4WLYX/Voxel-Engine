#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class GLProfile { Any, Core, Compat };

struct GLContext {
    int major = 3;
    int minor = 3;

    bool forward_compatible = true;
    GLProfile profile = GLProfile::Any;
};

struct WindowContext {
    std::string title = "Untitled";

    int width = 1280;
    int height = 720;

    int pos_x = GLFW_ANY_POSITION;
    int pos_y = GLFW_ANY_POSITION;

    bool resizable = true;
    bool visible = true;
    bool decorated = true;
    bool focused = true;
    bool auto_iconify = true;
    bool floating = false;
    bool maximized = false;
    bool center_cursor = false;
    bool transparent_framebuffer = false;
    bool focus_on_show = true;
    bool scale_to_monitor = false;
    bool scale_framebuffer = false;
    bool mouse_passthrough = false;

    GLContext opengl{};
};

class Window {
  public:
    Window(const WindowContext&);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void set_vsync(bool);

    void set_resize_callback(void(GLFWwindow*, int, int));

    GLFWwindow* handle() const;

    int width() const;
    int height() const;

    bool focused() const;

    bool should_close() const;

    void poll_events();
    void swap_buffers();

    void make_current();
    void clear();

  private:
    GLFWwindow* _window_handle = nullptr;

    int _width;
    int _height;

    int profile(GLProfile);
};

inline Window::Window(const WindowContext& ctx) : _width(ctx.width), _height(ctx.height) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ctx.opengl.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ctx.opengl.minor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, ctx.opengl.forward_compatible);
    glfwWindowHint(GLFW_OPENGL_PROFILE, profile(ctx.opengl.profile));

    glfwWindowHint(GLFW_POSITION_X, ctx.pos_y);
    glfwWindowHint(GLFW_POSITION_Y, ctx.pos_y);
    glfwWindowHint(GLFW_RESIZABLE, ctx.resizable);
    glfwWindowHint(GLFW_VISIBLE, ctx.visible);
    glfwWindowHint(GLFW_DECORATED, ctx.decorated);
    glfwWindowHint(GLFW_FOCUSED, ctx.focused);
    glfwWindowHint(GLFW_AUTO_ICONIFY, ctx.auto_iconify);
    glfwWindowHint(GLFW_FLOATING, ctx.floating);
    glfwWindowHint(GLFW_MAXIMIZED, ctx.maximized);
    glfwWindowHint(GLFW_CENTER_CURSOR, ctx.center_cursor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, ctx.transparent_framebuffer);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, ctx.focus_on_show);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, ctx.scale_to_monitor);
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, ctx.scale_framebuffer);
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, ctx.mouse_passthrough);

    _window_handle = glfwCreateWindow(ctx.width, ctx.height, ctx.title.c_str(), NULL, NULL);
    if (!_window_handle) {
        const char* buf;
        int error = glfwGetError(&buf);
        std::stringstream ss;
        ss << "Error " << error << ": " << buf;
        throw std::runtime_error(ss.str());
    }

    if (ctx.pos_x != GLFW_ANY_POSITION || ctx.pos_y != GLFW_ANY_POSITION)
        glfwSetWindowPos(_window_handle, ctx.pos_x, ctx.pos_y);
}

inline Window::~Window() { glfwDestroyWindow(_window_handle); }

inline void Window::set_vsync(bool state) { glfwSwapInterval(state ? 1 : 0); }

inline void Window::set_resize_callback(void (*callback)(GLFWwindow*, int, int)) {
    glfwSetFramebufferSizeCallback(_window_handle, callback);
}

inline GLFWwindow* Window::handle() const { return _window_handle; }

inline int Window::width() const { return _width; };

inline int Window::height() const { return _height; };

inline bool Window::focused() const { return glfwGetWindowAttrib(_window_handle, GLFW_FOCUSED); };

inline bool Window::should_close() const { return glfwWindowShouldClose(_window_handle); }

inline void Window::poll_events() { glfwPollEvents(); }

inline void Window::swap_buffers() { glfwSwapBuffers(_window_handle); }

inline void Window::make_current() { glfwMakeContextCurrent(_window_handle); }

inline void Window::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

inline int Window::profile(GLProfile profile) {
    // clang-format off
    switch (profile) {
        case GLProfile::Any   : return GLFW_OPENGL_ANY_PROFILE   ;
        case GLProfile::Core  : return GLFW_OPENGL_CORE_PROFILE  ;
        case GLProfile::Compat: return GLFW_OPENGL_COMPAT_PROFILE;
    }
    // clang-format on
}