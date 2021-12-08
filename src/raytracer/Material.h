#pragma once

#include "Ray.h"
#include "Shape.h"
#include <glm/glm/glm.hpp>

namespace raytracer {

// A material handles updating the ray
// Takes in a collision object, which contains the normal, position of surface
class IMaterial {
    public:
        virtual bool CastRay(Ray &ray, const Collision &collision) = 0;
};

class Metal: public IMaterial {
    private:
        glm::vec3 m_albedo;
        float m_fuzziness{1.0f};
    public:
        Metal(const glm::vec3 &albedo, float fuzziness);
        virtual bool CastRay(Ray &ray, const Collision &collision);
};

class Lambertian: public IMaterial {
    private:
        glm::vec3 m_albedo;
    public:
        Lambertian(const glm::vec3& albedo);
        virtual bool CastRay(Ray &ray, const Collision &collision);
};

class Dielectric: public IMaterial {
    private:
        float m_refractive_index;
        glm::vec3 m_color;
    public:
        Dielectric(float refractive_index, const glm::vec3 &color = glm::vec3{1,1,1});
        virtual bool CastRay(Ray &ray, const Collision &collision);
};

}