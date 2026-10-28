// © Joseph Cameron - MIT License

#include <gdk/input/gamepad.h>
#include <gdk/input/glfw_context.h>
#include <gdk/input/context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace gdk;

namespace {
    [[nodiscard]] std::shared_ptr<GLFWwindow> make_window() {
        glfwSetErrorCallback([](int, const char *const aMessage) {
            std::cerr << "glfw: " << aMessage << "\n";
        });

        if (!glfwInit()) throw std::runtime_error("glfwInit failed");

        std::shared_ptr<GLFWwindow> pWindow(
            glfwCreateWindow(400, 300, "gdk-input", nullptr, nullptr),
            [](GLFWwindow *const p) { if (p) glfwDestroyWindow(p); });

        if (!pWindow) throw std::runtime_error("could not create a window");

        glfwMakeContextCurrent(pWindow.get());

        return pWindow;
    }

    std::shared_ptr<GLFWwindow> pWindow;
    std::shared_ptr<input::glfw_context> pInput;

    [[nodiscard]] bool do_frame() {
        glfwPollEvents();
        pInput->update();

        if (pInput->key_just_released(input::keyboard::key::escape)) return false;

        if (const auto key = pInput->any_key_down()) std::cout << "key " << *key << "\n";

        if (const auto button = pInput->any_mouse_button_down())
            std::cout << "mouse " << *button << "\n";

        if (const auto delta = pInput->mouse_delta(); delta.x != 0 || delta.y != 0)
            std::cout << "moved " << delta.x << ", " << delta.y << " pixels\n";

        if (const auto scroll = pInput->mouse_scroll_delta(); scroll.y != 0 || scroll.x != 0)
            std::cout << "scrolled " << scroll.x << ", " << scroll.y << "\n";

        if (const auto pGamepad = pInput->get_gamepad(0); pGamepad->connected())
            if (const auto button = pGamepad->any_button_down())
                std::cout << "gamepad " << pGamepad->name() << " button " << *button << "\n";

        return true;
    }
}

int main() {
    pWindow = make_window();
    pInput = input::glfw_context::make(pWindow);

    std::cout << "hold a key, click, scroll, or move the mouse. escape quits.\n";

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop([] { if (!do_frame()) emscripten_cancel_main_loop(); }, 0, 1);
#else
    while (!glfwWindowShouldClose(pWindow.get()) && do_frame()) {}
#endif

    return EXIT_SUCCESS;
}
