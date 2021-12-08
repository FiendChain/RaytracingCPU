#pragma once

#include <vector>
#include "Camera.h"
#include "Scene.h"
#include "cptl_stl.h"

namespace raytracer
{

class Renderer {
    public:
        enum State { RUNNING, IDLE };
    public:
        Renderer(int total_threads=std::thread::hardware_concurrency());
        void Start(Camera &camera, Scene &scene, uint8_t *buffer, int width, int height); 
        void Stop();
        State GetState() { return m_state; }
    public:
        void RenderToBuffer(
            Camera &camera, Scene &scene, 
            uint8_t *buffer, int width, int height, 
            int x_start, int x_end, int y_start, int y_end);

        int m_total_bounces{4};
        int m_total_samples{1};
    private:
        State m_state;
        ctpl::thread_pool m_thread_pool;
};

}
