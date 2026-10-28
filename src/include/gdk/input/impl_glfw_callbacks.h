// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_IMPL_GLFW_CALLBACKS_H
#define GDK_INPUT_IMPL_GLFW_CALLBACKS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <unordered_map>

namespace gdk::input::glfw_detail {
    //! whose callbacks a window's are, one receiver of each type per window
    template <typename receiver_type>
    [[nodiscard]] std::unordered_map<GLFWwindow *, receiver_type *> &receivers() {
        static std::unordered_map<GLFWwindow *, receiver_type *> instance;

        return instance;
    }

    template <typename receiver_type>
    [[nodiscard]] receiver_type *receiver(GLFWwindow *const pWindow) {
        const auto found = receivers<receiver_type>().find(pWindow);

        return found == receivers<receiver_type>().end() ? nullptr : found->second;
    }

    //! the window's receiver becomes this one; returns the one it replaced, for \ref release
    template <typename receiver_type>
    [[nodiscard]] receiver_type *claim(GLFWwindow *const pWindow, receiver_type *const pReceiver) {
        auto &all = receivers<receiver_type>();

        receiver_type *const pPrevious = receiver<receiver_type>(pWindow);

        all[pWindow] = pReceiver;

        return pPrevious;
    }

    template <typename receiver_type>
    void release(GLFWwindow *const pWindow, receiver_type *const pReceiver, receiver_type *const pPrevious) {
        auto &all = receivers<receiver_type>();

        const auto found = all.find(pWindow);

        if (found == all.end() || found->second != pReceiver) return;

        if (pPrevious) found->second = pPrevious;
        else all.erase(found);
    }

    //! the callback that was there, unless it is ours: another of ours on the same window, which would call itself
    template <typename callback_type>
    [[nodiscard]] callback_type other_than(const callback_type aPrevious, const callback_type aOurs) {
        return aPrevious == aOurs ? nullptr : aPrevious;
    }

    //! put back the callback ours replaced, unless something has since replaced ours
    template <typename callback_type>
    void give_back(GLFWwindow *const pWindow, callback_type (*const aSet)(GLFWwindow *, callback_type),
        const callback_type aOurs, const callback_type aPrevious) {
        const auto current = aSet(pWindow, aPrevious);

        if (current != aOurs) aSet(pWindow, current);
    }
}

#endif
