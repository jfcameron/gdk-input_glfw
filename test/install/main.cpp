// © Joseph Cameron - MIT License

#include <gdk/input/controls.h>
#include <gdk/input/glfw_context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>

int main() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);

    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    int result = 1;

    if (auto *const pRaw = glfwCreateWindow(64, 64, "installed", nullptr, nullptr)) {
        const auto pContext = gdk::input::glfw_context::make(std::shared_ptr<GLFWwindow>(pRaw, glfwDestroyWindow));
        const auto pControls = gdk::input::controls::make(pContext);

        pControls->bind("jump", gdk::input::keyboard::key::space);

        glfwPollEvents();
        pContext->update();

        result = pControls->down("jump") ? 1 : 0;
    }

    glfwTerminate();

    return result;
}
