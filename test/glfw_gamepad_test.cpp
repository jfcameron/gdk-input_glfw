// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "fake_joysticks.h"
#include "test_include.h"

using namespace gdk::input;

namespace {
    struct pad_fixture final {
        pad_fixture() { fake_joysticks::reset(); }
        ~pad_fixture() { fake_joysticks::reset(); }

        input_fixture input;
    };
}

TEST_CASE("a pad plugged back in returns to its player", "[gamepad][glfw]") {
    pad_fixture f;

    fake_joysticks::plug(0, fake_joysticks::standard_pad("guid-a"));
    fake_joysticks::plug(1, fake_joysticks::standard_pad("guid-b"));
    f.input.poll();

    REQUIRE(f.input.pContext->get_gamepad(0)->guid() == "guid-a");
    REQUIRE(f.input.pContext->get_gamepad(1)->guid() == "guid-b");

    fake_joysticks::unplug(0);
    f.input.poll();

    REQUIRE_FALSE(f.input.pContext->get_gamepad(0)->connected());
    REQUIRE(f.input.pContext->get_gamepad(0)->just_disconnected());
    REQUIRE(f.input.pContext->get_gamepad(1)->connected());

    fake_joysticks::plug(2, fake_joysticks::standard_pad("guid-a"));
    f.input.poll();

    REQUIRE(f.input.pContext->get_gamepad(0)->connected());
    REQUIRE(f.input.pContext->get_gamepad(0)->just_connected());
    REQUIRE(f.input.pContext->get_gamepad(0)->guid() == "guid-a");

    SECTION("and players can trade pads") {
        f.input.pContext->swap_players(0, 1);

        REQUIRE(f.input.pContext->get_gamepad(0)->guid() == "guid-b");
        REQUIRE(f.input.pContext->get_gamepad(1)->guid() == "guid-a");
    }
}

TEST_CASE("an unmapped device reads raw, hats and all", "[gamepad][glfw]") {
    pad_fixture f;

    fake_joysticks::device stick;

    stick.guid = "guid-stick";
    stick.name = "flight stick";
    stick.standard = false;
    stick.axes = {0.5f, -0.25f, -1};
    stick.buttons = {GLFW_RELEASE, GLFW_PRESS, GLFW_RELEASE, GLFW_RELEASE};
    stick.hats = {GLFW_HAT_UP | GLFW_HAT_LEFT};

    fake_joysticks::plug(0, stick);
    f.input.poll();
    f.input.poll();

    const auto pGamepad = f.input.pContext->get_gamepad(0);

    REQUIRE(pGamepad->connected());
    REQUIRE_FALSE(pGamepad->has_standard_mapping());
    REQUIRE(pGamepad->name() == "flight stick");
    REQUIRE(pGamepad->axis_count() == 3);
    REQUIRE(pGamepad->button_count() == 4);
    REQUIRE(pGamepad->axis_value(gamepad::index_type{2}) == Approx(-1));
    REQUIRE(pGamepad->axis_value(gamepad::axis::left_x) == 0);
    REQUIRE(pGamepad->button_down(gamepad::index_type{1}));
    REQUIRE_FALSE(pGamepad->button_down(gamepad::button::b));

    REQUIRE(pGamepad->hat_count() == 1);
    REQUIRE(pGamepad->hat(0).y == gamepad::hat_state_type::vertical::up);
    REQUIRE(pGamepad->hat(0).x == gamepad::hat_state_type::horizontal::left);
}
