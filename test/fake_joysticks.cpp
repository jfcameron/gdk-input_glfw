// © Joseph Cameron - MIT License

#include "fake_joysticks.h"

#include <gdk/input/impl_joysticks.h>

#include <map>
#include <stdexcept>

using namespace gdk::input;

namespace {
    [[nodiscard]] std::map<int, fake_joysticks::device> &plugged() {
        static std::map<int, fake_joysticks::device> instance;

        return instance;
    }
}

fake_joysticks::device fake_joysticks::standard_pad(const std::string &aGuid) {
    device out;

    out.guid = aGuid;
    out.name = "fake pad";
    out.axes = {0, 0, 0, 0, -1, -1};
    out.buttons.assign(15, 0);

    return out;
}

void fake_joysticks::plug(const int aJoystick, const device &aDevice) { plugged()[aJoystick] = aDevice; }

void fake_joysticks::unplug(const int aJoystick) { plugged().erase(aJoystick); }

fake_joysticks::device &fake_joysticks::at(const int aJoystick) {
    const auto found = plugged().find(aJoystick);

    if (found == plugged().end()) throw std::out_of_range("no fake joystick plugged in there");

    return found->second;
}

void fake_joysticks::reset() { plugged().clear(); }

std::size_t gdk::input::joystick_count() { return 16; }

void gdk::input::sample_joysticks() {}

bool gdk::input::joystick_present(const int aJoystick) { return plugged().count(aJoystick) != 0; }

std::string gdk::input::joystick_guid(const int aJoystick) {
    return joystick_present(aJoystick) ? plugged()[aJoystick].guid : std::string();
}

joystick_reading gdk::input::read_joystick(const int aJoystick) {
    joystick_reading out;

    if (!joystick_present(aJoystick)) return out;

    const auto &device = plugged()[aJoystick];

    out.present = true;
    out.standard = device.standard;
    out.name = device.name;
    out.axes = device.axes;
    out.buttons = device.buttons;
    out.hats = device.hats;

    return out;
}

bool gdk::input::update_gamepad_mappings(const std::string &) { return true; }
