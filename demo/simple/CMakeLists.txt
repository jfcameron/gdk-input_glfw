# © Joseph Cameron - MIT License

add_executable(gdkinput_glfw_simple_demo "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(gdkinput_glfw_simple_demo PRIVATE gdkinput_glfw)

target_compile_features(gdkinput_glfw_simple_demo PRIVATE cxx_std_20)

if (EMSCRIPTEN)
    set_target_properties(gdkinput_glfw_simple_demo PROPERTIES SUFFIX ".html")
endif()
