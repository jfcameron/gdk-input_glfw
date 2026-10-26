# gdk-input_glfw
The [gdk-input](https://github.com/jfcameron/gdk-input) interface, implemented over [GLFW](https://www.glfw.org/): a window's keyboard, mouse and text input (with input method composition, through the GLFW fork's IME branch), and the joysticks and gamepads GLFW sees, on Linux, Windows, macOS and FreeBSD, and in the browser through Emscripten.

API documentation: https://jfcameron.github.io/gdk-input_glfw/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler. On Linux, GLFW needs the X11 and Wayland development headers (on Debian and Ubuntu: `libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev libxkbcommon-dev libwayland-dev wayland-protocols`).
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library, its tests and the demos on linux using gcc
cmake --preset linux-gcc

# build them
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
The tests run GLFW on its null platform and feed it events as the operating system would, so they need no display and run on every CI runner. One test program also replaces GLFW's joysticks with a fake set, so plugging pads in and out is tested too; it runs gdk-input's conformance suite.

See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::input_glfw`. It brings gdk-input and GLFW with it, unless the project already has them. Or install it (`cmake --install`, which installs gdk-input and GLFW beside it) and use `find_package(gdk-input_glfw)`.

## Usage
```cpp
#include <gdk/input/controls.h>
#include <gdk/input/glfw_context.h>

using namespace gdk::input;

// pWindow: a std::shared_ptr<GLFWwindow>, for example gdk-windowing_glfw's ptr_to_implementation()
const auto pInput = glfw_context::make(pWindow);
const auto pControls = controls::make(pInput);

pControls->bind("jump", keyboard::key::space);

while (!glfwWindowShouldClose(pWindow.get())) {
    glfwPollEvents();
    pInput->update();

    if (pControls->just_pressed("jump")) { /* jump */ }
}
```

The context takes the window's key, character, cursor and scroll callbacks, and passes each event on to the callback it replaced, so another library can still listen. When it goes, it gives them back.

`demo/` has two programs: `simple` prints what is pressed, and `rebinding` binds actions, captures a press to rebind one, and joins pads to players.
