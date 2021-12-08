#include "Renderer.h"

#include <numeric>
#include <assert.h>

namespace raytracer
{

Renderer::Renderer(int total_threads) 
: m_state(Renderer::State::IDLE),
  m_thread_pool(total_threads)
{

}

void Renderer::Start(Camera &camera, Scene &scene, uint8_t *buffer, int width, int height) {
    Stop();

    m_state = State::RUNNING;

    int total_threads = m_thread_pool.size();
    int nb_y_slices = total_threads*2;
    int nb_x_slices = total_threads*2;
    int y_slice = height / nb_y_slices;
    int x_slice = width / nb_x_slices;

    auto render_func = [this, &camera, &scene, buffer, width, height](int x_start, int x_end, int y_start, int y_end) {
        RenderToBuffer(camera, scene, buffer, width, height, x_start, x_end, y_start, y_end);
    };

    for (int ix = 0; ix < nb_x_slices; ix++) {
        for (int iy = 0; iy < nb_y_slices; iy++) {
            int y_start = y_slice*iy;
            int y_end = (iy < nb_y_slices-1) ? y_slice*(iy+1) : height;

            int x_start = x_slice*ix;
            int x_end = (ix < nb_x_slices-1) ? x_slice*(ix+1) : width;

            m_thread_pool.push([this, &camera, &scene, buffer, width, height, x_start, x_end, y_start, y_end](int id) {
                RenderToBuffer(camera, scene, buffer, width, height, x_start, x_end, y_start, y_end);
            });
        }
    }
}

void Renderer::Stop() {
    m_state = State::IDLE;
    m_thread_pool.clear_queue();
}

void Renderer::RenderToBuffer(
    Camera &camera, Scene &scene, 
    uint8_t *buffer, int width, int height, 
    int x_start, int x_end, int y_start, int y_end)
{
    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            if (m_state != State::RUNNING) {
                return;
            }

            glm::vec3 color{0,0,0};

            // get N samples
            for (int j = 0; j < m_total_samples; j++) {
                float s = (float)x / (float)(width-1);
                float t = 1.0f - (float)y / (float)(height-1);

                Ray ray = camera.GetRay(s, t);
                ray.color = glm::vec3{1, 1, 1};

                for (int i = 0; i <= m_total_bounces; i++) {
                    // out of bounces
                    if (i == m_total_bounces) {
                        ray.color *= glm::vec3{0,0,0};
                        break;
                    }

                    // if didn't hit anything in the scene
                    Scene::CastResult r = scene.CastRay(ray);
                    if (!r.no_bounce) {
                        break;
                    }

                    if (!r.hit_object) {
                        float t = 0.5f * (ray.direction.y + 1.0f);
                        glm::vec3 sky_color = (1.0f - t) * glm::vec3{1.0, 1.0, 1.0} + t * glm::vec3{0.5, 0.7, 1.0};
                        ray.color *= sky_color;
                        break;
                    }
                }
                
                color += ray.color;
            }

            color /= m_total_samples;

            // store color inside our buffer
            int i = (x + y*width) * 4;
            color = glm::sqrt(color);
            color = glm::clamp(color, 0.0f, 1.0f);
            buffer[i+0] = static_cast<uint8_t>(255 * color.r);
            buffer[i+1] = static_cast<uint8_t>(255 * color.g);
            buffer[i+2] = static_cast<uint8_t>(255 * color.b);
            buffer[i+3] = 255;
        }
    }

    // if about to become last task to finish, trigger flag
    if (m_thread_pool.n_idle() == m_thread_pool.size()-1) {
        m_state = State::IDLE;
    }
}

}