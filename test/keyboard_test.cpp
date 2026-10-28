// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "test_include.h"

#include <magic_enum/magic_enum.hpp>

#include <sstream>
#include <string>

using namespace gdk::input;

TEST_CASE("every key is mapped", "[keyboard]") {
    input_fixture f;

    for (const auto key : magic_enum::enum_values<keyboard::key>()) {
        const std::string name(magic_enum::enum_name(key));

        INFO("key: " << name);

        REQUIRE_NOTHROW(f.pContext->key_down(key));
        REQUIRE_NOTHROW(f.pContext->key_just_pressed(key));
        REQUIRE_NOTHROW(f.pContext->key_just_released(key));
    }
}

TEST_CASE("no key is down without input", "[keyboard]") {
    input_fixture f;

    SECTION("every key reads up") {
        for (const auto key : magic_enum::enum_values<keyboard::key>()) {
            INFO("key: " << std::string(magic_enum::enum_name(key)));

            REQUIRE_FALSE(f.pContext->key_down(key));
            REQUIRE_FALSE(f.pContext->key_just_pressed(key));
            REQUIRE_FALSE(f.pContext->key_just_released(key));
        }
    }

    SECTION("any_key_down reports nothing") {
        REQUIRE_FALSE(f.pContext->any_key_down().has_value());
    }

    SECTION("polling repeatedly does not invent input") {
        for (int i = 0; i < 5; ++i) {
            f.poll();

            REQUIRE_FALSE(f.pContext->any_key_down().has_value());
            REQUIRE_FALSE(f.pContext->key_down(keyboard::key::space));
        }
    }
}

TEST_CASE("the keypad's slash is its own key", "[keyboard]") {
    input_fixture f;

    input_glfw_key(f.pWindow.get(), GLFW_KEY_SLASH, GLFW_PRESS);
    f.poll();

    REQUIRE(f.pContext->key_down(keyboard::key::forwardslash));
    REQUIRE_FALSE(f.pContext->key_down(keyboard::key::numslash));

    input_glfw_key(f.pWindow.get(), GLFW_KEY_SLASH, GLFW_RELEASE);
    input_glfw_key(f.pWindow.get(), GLFW_KEY_KP_DIVIDE, GLFW_PRESS);
    f.poll();

    REQUIRE(f.pContext->key_down(keyboard::key::numslash));
    REQUIRE_FALSE(f.pContext->key_down(keyboard::key::forwardslash));
}

TEST_CASE("the menu key is GLFW's", "[keyboard]") {
    input_fixture f;

    input_glfw_key(f.pWindow.get(), GLFW_KEY_MENU, GLFW_PRESS);
    f.poll();

    REQUIRE(f.pContext->key_just_pressed(keyboard::key::menu));
}
