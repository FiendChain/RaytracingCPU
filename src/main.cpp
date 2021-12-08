#include <gl/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h> 

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <chrono>
#include <cmath>

#include <raytracer/Renderer.h>
#include <raytracer/Scene.h>
#include <raytracer/Camera.h>
#include <raytracer/Entity.h>

#include <glm/glm/glm.hpp>

#include "load_scene.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1700, 900, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // render the ray tracer output using opengl
    bool show_render_window = true;
    bool show_camera_window = true;
    // we can scale the image using opengl
    float scale = 1.0f;

    // create image buffer
    const unsigned int image_width = 1280;
    const unsigned int image_height = 720;
    uint8_t *image_data = new uint8_t[image_width*image_height*4];

    // create opengl texture
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    // setup the renderer
    auto renderer = new raytracer::Renderer();
    renderer->m_total_bounces = 8;
    renderer->m_total_samples = 10;
    
    // default camera setup
    auto camera = new raytracer::Camera();
    camera->m_vertical_fov = 45.0f;
    camera->m_aspect_ratio = (float)image_width/(float)image_height;
    camera->m_plane_distance = 10.0f;

    camera->m_look_from = glm::vec3{13,2,3};
    camera->m_look_at = glm::vec3{0,0,0};
    camera->m_up = glm::vec3{0,1,0};

    auto scene = new raytracer::Scene();
    load_scene(*scene);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Application statistics and main window
        {
            ImGui::Begin("Application Stats");                          
            ImGui::Checkbox("Render Window", &show_render_window);
            ImGui::Checkbox("Camera Controls", &show_camera_window);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); 
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // camera controls
        if (show_camera_window) {
            ImGui::Begin("Camera Controls", &show_camera_window);
            // camera settings
            ImGui::SliderFloat3("Look From", &(camera->m_look_from.x), -30.0f, 30.0f);
            ImGui::SliderFloat3("Look At", &(camera->m_look_at.x), -30.0f, 30.0f);
            ImGui::SliderFloat("Plane distance", &(camera->m_plane_distance), 1.0f, 100.0f);
            ImGui::SliderFloat("Vertical FOV", &(camera->m_vertical_fov), 1.0f, 100.0f);
            ImGui::End();
        }

        // render window
        if (show_render_window) {
            ImGui::Begin("Render Window", &show_render_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            const bool startDisable = renderer->GetState() == raytracer::Renderer::State::RUNNING;

            // start button
            if (startDisable) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Start!")) {
                memset(image_data, 0, static_cast<size_t>(image_width*image_height*4));
                camera->RecalculateVirtualPlane();
                renderer->Start(*camera, *scene, image_data, image_width, image_height);
            }
            if (startDisable) {
                ImGui::EndDisabled();
            }

            // stop button
            if (startDisable) {
                ImGui::SameLine();
                if (ImGui::Button("Abort")) {
                    renderer->Stop();
                }
            }

            // renderer settings
            ImGui::SliderFloat("Scale", &scale, 1.0f, 4.0f);            
            ImGui::SliderInt("Max Bounces", &(renderer->m_total_bounces), 1, 20);
            ImGui::SliderInt("Total samples", &(renderer->m_total_samples), 1, 50);
            {
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            }
            ImGui::Image((void*)(intptr_t)texture_id, ImVec2(std::floor(image_width*scale), std::floor(image_height*scale)));
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    renderer->Stop();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
