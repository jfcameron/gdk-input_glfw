// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_GLFW_CONTEXT_H
#define GDK_INPUT_GLFW_CONTEXT_H

#include <gdk/input/context.h>

#include <cstddef>
#include <memory>

struct GLFWwindow;

namespace gdk::input {
    /// \brief a GLFW window's keyboard, mouse and text input, and the joysticks GLFW sees
    class glfw_context final : public gdk::input::context {
    public:
        //! shares ownership of the window; takes its key, character, cursor and scroll callbacks, passing each on to the one it replaced
        [[nodiscard]] static std::shared_ptr<glfw_context> make(std::shared_ptr<GLFWwindow> apGLFWWindow);

        /// \brief take what happened since the last update: call once a frame, after glfwPollEvents
        void update();

        [[nodiscard]] virtual bool key_down(const keyboard::key &aKeyCode) const override;
        [[nodiscard]] virtual bool key_just_pressed(const keyboard::key &aKeyCode) const override;
        [[nodiscard]] virtual bool key_just_released(const keyboard::key &aKeyCode) const override;

        [[nodiscard]] virtual bool mouse_button_down(const mouse::button &aButton) const override;
        [[nodiscard]] virtual bool mouse_button_just_pressed(const mouse::button &aButton) const override;
        [[nodiscard]] virtual bool mouse_button_just_released(const mouse::button &aButton) const override;
        [[nodiscard]] virtual std::optional<mouse::axis> any_mouse_axis_down(const value_type aThreshold) const override;
        [[nodiscard]] virtual mouse::cursor_2d_type mouse_cursor_position() const override;
        [[nodiscard]] virtual mouse::cursor_2d_type mouse_delta() const override;
        [[nodiscard]] virtual mouse::scroll_2d_type mouse_scroll_delta() const override;
        [[nodiscard]] virtual mouse::cursor_mode mouse_cursor_mode() const override;
        virtual void set_mouse_cursor_mode(mouse::cursor_mode mode) override;

        [[nodiscard]] virtual const std::vector<text::event> &text_events() const override;
        [[nodiscard]] virtual text::composition text_composition() const override;
        [[nodiscard]] virtual bool text_input_focus() const override;

        [[nodiscard]] virtual text::modifiers modifiers_held() const override;
        virtual void set_text_input_focus(const bool aFocus) override;
        virtual void set_text_input_caret(const text::caret &aCaret) override;

        [[nodiscard]] virtual std::string clipboard() const override;
        virtual void set_clipboard(const std::string &aText) override;

        [[nodiscard]] virtual gamepad_ptr get_gamepad(const std::size_t aIndex) override;
        [[nodiscard]] virtual gamepad_collection_type gamepads() override;
        virtual void swap_players(const std::size_t aLeft, const std::size_t aRight) override;
        virtual std::size_t add_gamepad_mappings(const std::string &aMappings) override;

        virtual ~glfw_context() override;

    private:
        explicit glfw_context(std::shared_ptr<GLFWwindow> apGLFWWindow);

        struct impl;
        std::unique_ptr<impl> m_pImpl;
    };
}

#endif
