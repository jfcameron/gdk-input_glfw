// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_GLFW_FAKE_JOYSTICKS_H
#define GDK_INPUT_GLFW_FAKE_JOYSTICKS_H

#include <string>
#include <vector>

namespace gdk::input::fake_joysticks {
    struct device final {
        std::string guid;
        std::string name;

        bool standard{true};

        std::vector<float> axes;
        std::vector<unsigned char> buttons;
        std::vector<unsigned char> hats;
    };

    //! a pad on GLFW's gamepad layout: 15 buttons, 6 axes, the triggers -1 at rest
    [[nodiscard]] device standard_pad(const std::string &aGuid);

    void plug(const int aJoystick, const device &aDevice);
    void unplug(const int aJoystick);

    [[nodiscard]] device &at(const int aJoystick);

    void reset();
}

#endif
