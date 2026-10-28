// © Joseph Cameron - MIT License

#include "conformance.h"
#include "fake_joysticks.h"
#include "test_include.h"

#include <cstdint>
#include <stdexcept>
#include <string>

using namespace gdk::input;

namespace {
    [[nodiscard]] int glfw_key(const keyboard::key aKey) {
        switch (aKey) {
            case keyboard::key::space: return GLFW_KEY_SPACE;
            case keyboard::key::a: return GLFW_KEY_A;
            case keyboard::key::e: return GLFW_KEY_E;
            case keyboard::key::w: return GLFW_KEY_W;
            case keyboard::key::uparrow: return GLFW_KEY_UP;
            case keyboard::key::numslash: return GLFW_KEY_KP_DIVIDE;
            case keyboard::key::leftshift: return GLFW_KEY_LEFT_SHIFT;
            case keyboard::key::rightcontrol: return GLFW_KEY_RIGHT_CONTROL;
            default: throw std::invalid_argument("the glfw conformance driver has no key for that");
        }
    }

    [[nodiscard]] int glfw_button(const mouse::button aButton) {
        return GLFW_MOUSE_BUTTON_1 + static_cast<int>(aButton);
    }

    class glfw_driver final : public conformance::driver {
    public:
        glfw_driver() {
            fake_joysticks::reset();

            glfwGetCursorPos(m_pWindow.get(), &m_CursorX, &m_CursorY);
        }

        ~glfw_driver() override { fake_joysticks::reset(); }

        context_ptr_type context() override { return m_pContext; }

        void frame() override {
            glfwPollEvents();
            m_pContext->update();
        }

        void press(const keyboard::key aKey) override { key(aKey, GLFW_PRESS); }
        void release(const keyboard::key aKey) override { key(aKey, GLFW_RELEASE); }

        void press(const mouse::button aButton) override {
            _glfwInputMouseClick(internal(m_pWindow.get()), glfw_button(aButton), GLFW_PRESS, 0);
        }

        void release(const mouse::button aButton) override {
            _glfwInputMouseClick(internal(m_pWindow.get()), glfw_button(aButton), GLFW_RELEASE, 0);
        }

        void move_cursor(const double aX, const double aY) override {
            m_CursorX += aX;
            m_CursorY -= aY;

            _glfwInputCursorPos(internal(m_pWindow.get()), m_CursorX, m_CursorY);
        }

        void scroll(const double aX, const double aY) override {
            _glfwInputScroll(internal(m_pWindow.get()), aX, aY);
        }

        void type(std::string_view aText) override {
            for (std::size_t i = 0; i < aText.size();) {
                const auto lead = static_cast<unsigned char>(aText[i]);
                const std::size_t length = lead < 0x80 ? 1 : lead < 0xE0 ? 2 : lead < 0xF0 ? 3 : 4;

                std::uint32_t codePoint = length == 1 ? lead : lead & (0x7F >> length);

                for (std::size_t j = 1; j < length; ++j)
                    codePoint = (codePoint << 6) | (static_cast<unsigned char>(aText[i + j]) & 0x3F);

                _glfwInputChar(internal(m_pWindow.get()), codePoint, 0, GLFW_TRUE);

                i += length;
            }
        }

        bool attach_gamepad() override {
            fake_joysticks::plug(0, fake_joysticks::standard_pad("03000000fa1e0000000000000000ab01"));

            return true;
        }

        void detach_gamepad() override { fake_joysticks::unplug(0); }

        void press(const gamepad::button aButton) override {
            fake_joysticks::at(0).buttons.at(static_cast<std::size_t>(aButton)) = GLFW_PRESS;
        }

        void release(const gamepad::button aButton) override {
            fake_joysticks::at(0).buttons.at(static_cast<std::size_t>(aButton)) = GLFW_RELEASE;
        }

        void set_axis(const gamepad::axis aAxis, const float aValue) override {
            const bool trigger = aAxis == gamepad::axis::left_trigger || aAxis == gamepad::axis::right_trigger;

            fake_joysticks::at(0).axes.at(static_cast<std::size_t>(aAxis)) = trigger ? aValue * 2 - 1 : aValue;
        }

    private:
        void key(const keyboard::key aKey, const int aAction) {
            _glfwInputKey(internal(m_pWindow.get()), glfw_key(aKey), 0, aAction, 0);
        }

        std::shared_ptr<GLFWwindow> m_pWindow = initGLFW();
        std::shared_ptr<glfw_context> m_pContext = glfw_context::make(m_pWindow);

        double m_CursorX = 0;
        double m_CursorY = 0;
    };
}

namespace gdk::input::conformance {
    std::unique_ptr<driver> make_driver() { return std::make_unique<glfw_driver>(); }

    std::string_view implementation() { return "glfw"; }
}
