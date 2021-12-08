#include "Camera.h"

#include <cmath>

namespace raytracer
{

void Camera::RecalculateVirtualPlane() {
    float theta = m_vertical_fov * 3.1415f/180.0f;
    float h = std::tan(theta/2);
    float viewport_height = h * 2.0f;
    float viewport_width = m_aspect_ratio * viewport_height;

    // get the 3-axis for our viewport 
    // w = origin to target vector
    // u = horizontal vector
    // v = vertical vector
    glm::vec3 w = glm::normalize(m_look_from - m_look_at);
    glm::vec3 u = glm::normalize(glm::cross(m_up, w));
    glm::vec3 v = glm::cross(w, u);

    // set the virtual camera image plane
    m_origin = m_look_from;
    m_horizontal = m_plane_distance * viewport_width*u;
    m_vertical = m_plane_distance * viewport_height*v;
    m_lower_left = m_origin - m_horizontal/2.0f - m_vertical/2.0f - m_plane_distance*w;
}

Ray Camera::GetRay(float s, float t) {
    Ray ray;
    ray.color = glm::vec3{0, 0, 0};
    ray.origin = m_origin;

    glm::vec3 screen_pos = m_lower_left + m_horizontal*s + m_vertical*t;
    ray.direction = glm::normalize(screen_pos - m_origin);

    return ray;
}

}