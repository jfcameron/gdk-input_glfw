# © Joseph Cameron - MIT License

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/gamepad_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/keyboard_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mouse_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/text_input_test.cpp"

    INCLUDE_DIRECTORIES
        "${PROJECT_SOURCE_DIR}/src/include"

    LIBRARIES
        gdkinput_glfw
        gdkinput_implementation)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        ${GDKINPUT_GLFW_SOURCES}
        "${CMAKE_CURRENT_LIST_DIR}/fake_joysticks.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/glfw_conformance.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/glfw_gamepad_test.cpp"

    INCLUDE_DIRECTORIES
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src/include"

    LIBRARIES
        glfw
        gdkinput_implementation
        gdkinput_conformance)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

if (NOT CMAKE_CROSSCOMPILING AND GDKINPUT_GLFW_INSTALL)
    add_test(NAME gdk-input_glfw_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
