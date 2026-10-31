# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkinput_glfw)
    return()
endif()

project(gdk-input_glfw
    VERSION 1.0
    DESCRIPTION "the gdk input interface, implemented over GLFW"
    LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKINPUT_GLFW_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKINPUT_GLFW_BUILD_DEMO "Build the demos" OFF)
option(GDKINPUT_GLFW_BUILD_DOCS "Build documentation" OFF)
option(GDKINPUT_GLFW_INSTALL "Generate install rules, GLFW's included" ${PROJECT_IS_TOP_LEVEL})

if (GDKINPUT_GLFW_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
    )

    if (NOT GDKINPUT_GLFW_BUILD_TESTS AND NOT GDKINPUT_GLFW_BUILD_DEMO)
        return()
    endif()
endif()

add_subdirectory(extern)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(GDKINPUT_GLFW_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_gamepad.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_keyboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_mouse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_text_input.cpp")

add_library(gdkinput_glfw STATIC
    ${GDKINPUT_GLFW_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_joysticks.cpp")

add_library(gdk::input_glfw ALIAS gdkinput_glfw)

set_target_properties(gdkinput_glfw PROPERTIES EXPORT_NAME input_glfw PREFIX "lib")

target_include_directories(gdkinput_glfw
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_compile_features(gdkinput_glfw PUBLIC cxx_std_20)

target_link_libraries(gdkinput_glfw PUBLIC gdkinput PRIVATE $<BUILD_INTERFACE:gdkinput_implementation>)

if (EMSCRIPTEN)
    target_link_options(gdkinput_glfw PUBLIC "-sUSE_GLFW=3")
else()
    target_link_libraries(gdkinput_glfw PUBLIC glfw)
endif()

if (GDKINPUT_GLFW_INSTALL)
    install(TARGETS gdkinput_glfw
        EXPORT gdk-input_glfw-targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(EXPORT gdk-input_glfw-targets
        FILE gdk-input_glfw-targets.cmake
        NAMESPACE gdk::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-input_glfw")

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/gdk-input_glfw-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    set(_gdkinput_glfw_find_glfw "find_dependency(glfw3)\n")

    if (EMSCRIPTEN)
        set(_gdkinput_glfw_find_glfw "")
    endif()

    file(WRITE "${PROJECT_BINARY_DIR}/gdk-input_glfw-config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(gdk-input 1)
${_gdkinput_glfw_find_glfw}include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-input_glfw-targets.cmake\")
")

    install(FILES
        "${PROJECT_BINARY_DIR}/gdk-input_glfw-config.cmake"
        "${PROJECT_BINARY_DIR}/gdk-input_glfw-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-input_glfw")
endif()

if (GDKINPUT_GLFW_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKINPUT_GLFW_BUILD_DEMO)
    add_subdirectory(demo)
endif()
