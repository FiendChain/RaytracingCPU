#pragma once

#include <glm/glm/glm.hpp>
#include "Ray.h"

namespace raytracer {

class Camera {
    public:
        Camera() {};
        Ray GetRay(float s, float t);
        // before usage, run this to update virtual plane parameters
        void RecalculateVirtualPlane();
    
    // have this accessible to imgui
    public:
        float m_vertical_fov{70.0f};
        float m_aspect_ratio{1.7f};
        float m_plane_distance{10.0f};
        glm::vec3 m_look_from{0,0,0};
        glm::vec3 m_look_at{1,1,1};
        glm::vec3 m_up{0,1,0};
    private:
        // virtual plane parameters
        glm::vec3 m_horizontal, m_vertical;
        glm::vec3 m_origin;
        glm::vec3 m_lower_left;

};

}