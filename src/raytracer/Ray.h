#pragma once

#include <glm/glm/glm.hpp>

namespace raytracer 
{

struct Ray {
    public: 
        glm::vec3 origin;
        glm::vec3 direction;
        glm::vec3 color;
};

}