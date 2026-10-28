// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_IMPL_GLFW_MOUSE_H
#define GDK_INPUT_IMPL_GLFW_MOUSE_H

#include <gdk/input/types.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

namespace gdk::input {
    class mouse_glfw final {
    public:
        void update();

        [[nodiscard]] bool button_down(const mouse::button aButton) const;
        [[nodiscard]] bool button_just_down(const mouse::button aButton) const;
        [[nodiscard]] bool button_just_released(const mouse::button aButton) const;

        [[nodiscard]] mouse::cursor_2d_type cursor_position() const;
        [[nodiscard]] mouse::cursor_2d_type delta() const;
        [[nodiscard]] mouse::scroll_2d_type scroll_delta() const;

        void set_cursor_mode(const mouse::cursor_mode aMode);
        [[nodiscard]] mouse::cursor_mode get_cursor_mode() const;

        explicit mouse_glfw(std::shared_ptr<GLFWwindow> pWindow);

        ~mouse_glfw();

        mouse_glfw(const mouse_glfw &) = delete;
        mouse_glfw &operator=(const mouse_glfw &) = delete;

    private:
        static void on_cursor(GLFWwindow *pWindow, double aX, double aY);
        static void on_scroll(GLFWwindow *pWindow, double aX, double aY);

        [[nodiscard]] mouse::cursor_2d_type normalised(const mouse::cursor_2d_type aCursor) const;

        std::shared_ptr<GLFWwindow> m_pWindow;
        mouse::cursor_mode m_CursorMode = mouse::cursor_mode::normal;

        mouse::cursor_2d_type m_Cursor{0, 0};
        mouse::cursor_2d_type m_CursorAtUpdate{0, 0};
        mouse::cursor_2d_type m_Position{0, 0};
        mouse::cursor_2d_type m_Delta{0, 0};

        mouse::scroll_2d_type m_PendingScroll{0, 0};
        mouse::scroll_2d_type m_ScrollDelta{0, 0};

        std::unordered_map<int, int> m_MouseButtonLastState;
        std::unordered_map<int, mouse::button_state> m_CurrentState;

        mouse_glfw *m_pPreviousReceiver = nullptr;

        GLFWcursorposfun m_PreviousCursor = nullptr;
        GLFWscrollfun m_PreviousScroll = nullptr;
    };
}

#endif
