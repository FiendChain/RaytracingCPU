#include "Material.h"

#include <glm/glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/epsilon.hpp>

namespace raytracer {

Metal::Metal(const glm::vec3 &albedo, float fuzziness)
: m_albedo(albedo), m_fuzziness(fuzziness) 
{}

Lambertian::Lambertian(const glm::vec3 &albedo)
: m_albedo(albedo)
{}

Dielectric::Dielectric(float refractive_index, const glm::vec3 &color)
:   m_refractive_index(refractive_index),
    m_color(color)
{}


bool Metal::CastRay(Ray &ray, const Collision &collision) {
    // metallic scattering
    glm::vec3 pure_reflection = glm::reflect(ray.direction, collision.normal);
    glm::vec3 reflected = glm::normalize(
        pure_reflection +
        m_fuzziness*glm::sphericalRand(1.0f));
    
    if (glm::dot(pure_reflection, collision.normal) < 0) {
        reflected = glm::normalize(pure_reflection);
    }

    ray.origin = collision.pos;
    ray.direction = reflected;
    ray.color *= m_albedo;
    return true;
}

bool Lambertian::CastRay(Ray &ray, const Collision &collision) {
    // diffuse scattering
    glm::vec3 scatter = glm::normalize(
        collision.normal + 
        glm::sphericalRand(1.0f));

    if (glm::any(glm::epsilonEqual(scatter, glm::vec3{0,0,0}, std::numeric_limits<float>::epsilon()))) {
        scatter = collision.normal;
    }

    ray.origin = collision.pos;
    ray.direction = scatter;
    ray.color *= m_albedo;
    return true;
}

bool Dielectric::CastRay(Ray &ray, const Collision &collision) {
    // we go from medium 1 into medium 2
    // refraction_ratio = n_1 / n_2 (n = optical density)

    // check if total internal reflection
    // if it is internal, than the interface is reverse, therefore refractive index is inverted
    float refraction_ratio = collision.is_internal ? m_refractive_index : 1.0f/m_refractive_index;

    // get angle between ray and the surface normal
    float cos_theta = glm::min(glm::dot(-ray.direction, collision.normal), 1.0f);
    float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);

    // check if the ray refracts, or reflects
    // total internal reflection occurs when going from more dense to less dense
    // refraction_ratio > 1.0f for total internal reflection

    // sin(theta_t)/sin(theta_i) = n_1/n_2
    // For total internal reflection, theta_t = 90, sin(theta_t) = 1
    // sin(theta_t) = sin(theta_i) * n_1/n_2
    // Hence if sin(theta_i) * n_1/n_2 > 1.0f, we have total internal reflection

    bool is_refract = refraction_ratio*sin_theta < 1.0f;

    glm::vec3 direction;
    if (is_refract) {
        direction = glm::refract(ray.direction, collision.normal, refraction_ratio);
    } else {
        direction = glm::reflect(ray.direction, collision.normal);
    }

    ray.origin = collision.pos;
    ray.direction = direction;
    ray.color *= m_color;
    return true;
}

}