#pragma once

#include <glm/glm/glm.hpp>
#include "Ray.h"

namespace raytracer {

// information about where a ray hit a shape
struct Collision {
    glm::vec3 pos;
    glm::vec3 normal;
    bool is_internal; // so we can treat shapes as hollow
};

class IShape {
    public:
        virtual bool CheckHit(const Ray &ray, float &t0, float &t1) = 0;
        virtual Collision GetCollision(const Ray &ray, float t) = 0;
};

class Sphere: public IShape {
    private:
        glm::vec3 m_center;
        float m_radius;
    public:
        Sphere(glm::vec3 center, float radius); 
        virtual bool CheckHit(const Ray &ray, float &t0, float &t1);
        virtual Collision GetCollision(const Ray &ray, float t);
};

}