set(MAIN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(IMGUI_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm")

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glfw/include")

# glew
set (BUILD_UTILS OFF CACHE BOOL "" FORCE)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glew/build/cmake)

# raytracer
add_subdirectory(raytracer)

set(SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/load_scene.cpp")

add_executable(demo ${SOURCES})
target_include_directories(demo PUBLIC ${MAIN_SOURCE_DIR})
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(demo PUBLIC imgui glfw ${GLFW_LIBRARIES} glm opengl32 glew_s raytracer)
set_target_properties(demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

