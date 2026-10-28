// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_GLFW_TEST_INCLUDE_H
#define GDK_INPUT_GLFW_TEST_INCLUDE_H

#include <gdk/input/glfw_context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

extern "C" {
    void _glfwInputKey(struct _GLFWwindow *window, int key, int scancode, int action, int mods);
    void _glfwInputChar(struct _GLFWwindow *window, std::uint32_t codepoint, int mods, int plain);
    void _glfwInputScroll(struct _GLFWwindow *window, double xoffset, double yoffset);
    void _glfwInputMouseClick(struct _GLFWwindow *window, int button, int action, int mods);
    void _glfwInputCursorPos(struct _GLFWwindow *window, double xpos, double ypos);
}

[[nodiscard]] inline struct _GLFWwindow *internal(GLFWwindow *const pWindow) {
    return reinterpret_cast<struct _GLFWwindow *>(pWindow);
}

inline void input_glfw_key(GLFWwindow *const pWindow, const int aKey, const int aAction) {
    _glfwInputKey(internal(pWindow), aKey, 0, aAction, 0);
}

[[nodiscard]] inline std::shared_ptr<GLFWwindow> initGLFW() {
    static std::string lastError;

    glfwSetErrorCallback([](int, const char *const aMessage) { lastError = aMessage; });

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);

    if (!glfwInit()) throw std::runtime_error("glfwInit failed: " + lastError);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    std::shared_ptr<GLFWwindow> pWindow(glfwCreateWindow(320, 240, "gdkinput test", nullptr, nullptr),
        [](GLFWwindow *const p) { if (p) glfwDestroyWindow(p); });

    if (!pWindow) throw std::runtime_error("glfwCreateWindow failed: " + lastError);

    return pWindow;
}

struct input_fixture final {
    std::shared_ptr<GLFWwindow> pWindow = initGLFW();
    std::shared_ptr<gdk::input::glfw_context> pContext = gdk::input::glfw_context::make(pWindow);

    void poll() {
        glfwPollEvents();
        pContext->update();
    }

    input_fixture() { poll(); }
};

#endif
