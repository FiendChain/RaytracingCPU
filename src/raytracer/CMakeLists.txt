set(RAYTRACER_SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/Shape.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
)

add_library(raytracer STATIC ${RAYTRACER_SOURCES})
target_include_directories(raytracer PUBLIC ${MAIN_SOURCE_DIR})
target_include_directories(raytracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(raytracer PUBLIC glm)