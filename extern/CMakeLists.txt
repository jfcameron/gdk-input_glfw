# © Joseph Cameron - MIT License

if (NOT TARGET gdkinput)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-input")
endif()

if (TARGET glfw)
    return()
endif()

if (EMSCRIPTEN)
    add_library(glfw INTERFACE)

    target_link_options(glfw INTERFACE "-sUSE_GLFW=3")

    return()
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL ${GDKINPUT_GLFW_INSTALL} CACHE BOOL "" FORCE)

set(_gdkinput_glfw_shared "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")

set(BUILD_SHARED_LIBS "${_gdkinput_glfw_shared}")
