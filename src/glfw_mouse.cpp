// © Joseph Cameron - MIT License

#include <gdk/input/impl_glfw_mouse.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/em_asm.h>
#endif

#include <gdk/input/impl_glfw_callbacks.h>

#include <array>
#include <stdexcept>
#include <string>
#include <utility>

using namespace gdk::input;

static inline decltype(GLFW_MOUSE_BUTTON_1) glfwmouseButtonFromButton(const gdk::input::mouse::button a) {
    switch(a) {
        case gdk::input::mouse::button::left: return GLFW_MOUSE_BUTTON_LEFT;
        case gdk::input::mouse::button::right: return GLFW_MOUSE_BUTTON_RIGHT;
        case gdk::input::mouse::button::middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        case gdk::input::mouse::button::four: return GLFW_MOUSE_BUTTON_4;
        case gdk::input::mouse::button::five: return GLFW_MOUSE_BUTTON_5;
        case gdk::input::mouse::button::six: return GLFW_MOUSE_BUTTON_6;
        case gdk::input::mouse::button::seven: return GLFW_MOUSE_BUTTON_7;
        case gdk::input::mouse::button::eight: return GLFW_MOUSE_BUTTON_8;

		default: throw std::invalid_argument(std::string("Unable to convert mouse button \"")
			.append(std::to_string(static_cast<std::underlying_type< decltype(a)>::type>(a)))
			.append("\" to GLFW_MOUSE_BUTTON"));
    }
}

mouse_glfw::mouse_glfw(std::shared_ptr<GLFWwindow> pWindow)
: m_pWindow(std::move(pWindow)) {
    auto *const pGLFWWindow = m_pWindow.get();

    glfwGetCursorPos(pGLFWWindow, &m_Cursor.x, &m_Cursor.y);

    m_CursorAtUpdate = m_Cursor;
    m_Position = normalised(m_Cursor);

    m_pPreviousReceiver = glfw_detail::claim(pGLFWWindow, this);

    m_PreviousCursor = glfw_detail::other_than(glfwSetCursorPosCallback(pGLFWWindow, on_cursor), &on_cursor);
    m_PreviousScroll = glfw_detail::other_than(glfwSetScrollCallback(pGLFWWindow, on_scroll), &on_scroll);
}

mouse_glfw::~mouse_glfw() {
    auto *const pGLFWWindow = m_pWindow.get();

    if (!m_pPreviousReceiver) {
        glfw_detail::give_back<GLFWcursorposfun>(pGLFWWindow, glfwSetCursorPosCallback, on_cursor, m_PreviousCursor);
        glfw_detail::give_back<GLFWscrollfun>(pGLFWWindow, glfwSetScrollCallback, on_scroll, m_PreviousScroll);
    }

    glfw_detail::release(pGLFWWindow, this, m_pPreviousReceiver);
}

void mouse_glfw::on_cursor(GLFWwindow *const pWindow, const double aX, const double aY) {
    if (auto *const pMouse = glfw_detail::receiver<mouse_glfw>(pWindow)) {
        pMouse->m_Cursor = {aX, aY};

        if (pMouse->m_PreviousCursor) pMouse->m_PreviousCursor(pWindow, aX, aY);
    }
}

void mouse_glfw::on_scroll(GLFWwindow *const pWindow, const double aX, const double aY) {
    if (auto *const pMouse = glfw_detail::receiver<mouse_glfw>(pWindow)) {
        pMouse->m_PendingScroll.x += aX;
        pMouse->m_PendingScroll.y += aY;

        if (pMouse->m_PreviousScroll) pMouse->m_PreviousScroll(pWindow, aX, aY);
    }
}

mouse::cursor_2d_type mouse_glfw::normalised(const mouse::cursor_2d_type aCursor) const {
    int width = 0, height = 0;

    glfwGetWindowSize(m_pWindow.get(), &width, &height);

    if (width <= 0 || height <= 0) return m_Position;

    return {aCursor.x / width, 1 - aCursor.y / height};
}

void mouse_glfw::set_cursor_mode(const mouse::cursor_mode aMode) {
    decltype(GLFW_CURSOR_NORMAL) cursorModeBuffer;

    switch (aMode) {
        case mouse::cursor_mode::normal: cursorModeBuffer = GLFW_CURSOR_NORMAL; break;
        case mouse::cursor_mode::hidden: cursorModeBuffer = GLFW_CURSOR_HIDDEN; break;
        case mouse::cursor_mode::locked: cursorModeBuffer = GLFW_CURSOR_DISABLED; break;

        default: throw std::invalid_argument(std::string("Unhandled cursor_mode: ")
            .append(std::to_string(static_cast<std::underlying_type<decltype(aMode)>::type>(aMode))));
    }

#ifdef __EMSCRIPTEN__
    EM_ASM({
        const canvas = Module['canvas'];

        if (canvas) canvas.style.cursor = $0 ? "none" : "";
    }, aMode == mouse::cursor_mode::hidden);

    if (cursorModeBuffer == GLFW_CURSOR_HIDDEN) cursorModeBuffer = GLFW_CURSOR_NORMAL;
#endif

    glfwSetInputMode(m_pWindow.get(), GLFW_CURSOR, cursorModeBuffer);

    m_CursorMode = aMode;
}

mouse::cursor_mode mouse_glfw::get_cursor_mode() const {
    return m_CursorMode;
}

mouse::cursor_2d_type mouse_glfw::cursor_position() const { return m_Position; }

mouse::cursor_2d_type mouse_glfw::delta() const {
    return m_Delta;
}

mouse::scroll_2d_type mouse_glfw::scroll_delta() const {
    return m_ScrollDelta;
}

bool mouse_glfw::button_down(const mouse::button aButton) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aButton)); search != m_CurrentState.end())
        value = search->second == gdk::input::mouse::button_state::held_down ||
        search->second == gdk::input::mouse::button_state::just_pressed;
	
    return value;
}

bool mouse_glfw::button_just_down(const mouse::button aKeyCode) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::input::mouse::button_state::just_pressed;

    return value;
}

bool mouse_glfw::button_just_released(const mouse::button aKeyCode) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::input::mouse::button_state::just_released;

    return value;
}

void mouse_glfw::update() {
    static constexpr std::array<decltype(GLFW_MOUSE_BUTTON_LEFT), 8> BUTTON_SET{
        GLFW_MOUSE_BUTTON_LEFT,
        GLFW_MOUSE_BUTTON_RIGHT,
        GLFW_MOUSE_BUTTON_MIDDLE,
        GLFW_MOUSE_BUTTON_4,
        GLFW_MOUSE_BUTTON_5,
        GLFW_MOUSE_BUTTON_6,
        GLFW_MOUSE_BUTTON_7,
        GLFW_MOUSE_BUTTON_8,
    };

    for (const auto glfwButton : BUTTON_SET) {
        auto lastButtonState = m_MouseButtonLastState[glfwButton];
        auto currentButtonState = glfwGetMouseButton(m_pWindow.get(), glfwButton);
        auto newState = mouse::button_state::up;

        if (currentButtonState == GLFW_RELEASE && lastButtonState != GLFW_RELEASE)
            newState = mouse::button_state::just_released;
        else if (currentButtonState != GLFW_RELEASE)
            newState = lastButtonState == GLFW_RELEASE
                ? mouse::button_state::just_pressed
                : mouse::button_state::held_down;

        m_CurrentState[glfwButton] = newState;
        m_MouseButtonLastState[glfwButton] = currentButtonState;
    }

    m_Delta = {m_Cursor.x - m_CursorAtUpdate.x, m_CursorAtUpdate.y - m_Cursor.y};
    m_CursorAtUpdate = m_Cursor;
    m_Position = normalised(m_Cursor);

    m_ScrollDelta = std::exchange(m_PendingScroll, {0, 0});
}

