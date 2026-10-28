# © Joseph Cameron - MIT License

add_executable(gdkinput_glfw_rebinding_demo "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(gdkinput_glfw_rebinding_demo PRIVATE gdkinput_glfw)

target_compile_features(gdkinput_glfw_rebinding_demo PRIVATE cxx_std_20)

if (EMSCRIPTEN)
    set_target_properties(gdkinput_glfw_rebinding_demo PROPERTIES SUFFIX ".html")
endif()

if (TARGET gdkinput_ext_lua)
    target_link_libraries(gdkinput_glfw_rebinding_demo PRIVATE gdkinput_ext_lua)
    target_compile_definitions(gdkinput_glfw_rebinding_demo PRIVATE GDK_INPUT_DEMO_HAS_LUA=1)
else()
    target_compile_definitions(gdkinput_glfw_rebinding_demo PRIVATE GDK_INPUT_DEMO_HAS_LUA=0)
endif()
