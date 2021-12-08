#include "Shape.h"
#include "Ray.h"

namespace raytracer {

Sphere::Sphere(glm::vec3 center, float radius)
: m_center(center), m_radius(radius) 
{
}

float square_length(const glm::vec3 &v) {
    return glm::dot(v, v);
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool Sphere::CheckHit(const Ray &ray, float &t0, float &t1) {
    /*
    C = circle center vector
    r = radius
    parametric equation of sphere: ||x-C||^2 = r^2

    A = start position
    B = direction
    parametric equation of ray: x(t) = A + B*t

    solution for t
    ||A+B*t-C||^2 = r^2
    (A.x+B.x*t-C.x)^2 + ... = r^2
    (B.x^2)*(t^2) - 2*t*B.x*(A.x-C.x) + (A.x-C.x)^2 + .... = r^2
    ||B||^2 * t^2 - 2*dot(B, A-C)*t + ||A-C||^2 = r^2

    This is rewritten as a quadratic equation
    a*t^2 + b*t + c = 0
    a = ||B||^2
    b = -2*dot(B, A-C) 
    c = ||A-C||^2 - r^2
    discriminant = b^2 - 4*a*c = 4*[(b/2)^2 - a*c]
    
    we first check if discriminant has solution or not
    discriminant >= 0 means there is a solution, and our ray hits
    t = [-b ± sqrt(discriminant)] / (2*a)
    t = [-b/2 ± sqrt(discriminant/4)] / a

    to reduce the amount of computation, we can optimise this abit
    let D = discriminant/4
    D = (b/2)^2 - a*c
    t = [-b/2 ± sqrt(D)] / a
    let G = b/2
    D = G^2 - a*c
    t = [-G ± sqrt(D)] / a
    this is our final optimised equation
    */

    glm::vec3 delta_pos = ray.origin - m_center;

    const float a = square_length(ray.direction);
    const float c = square_length(delta_pos) - m_radius*m_radius;
    const float half_b = glm::dot(delta_pos, ray.direction);

    const float D = half_b*half_b - a*c;
    if (D < 0) {
        return false;
    }
    const float sqrt_D = glm::sqrt(D);

    // our intersection interval
    t0 = (-half_b - sqrt_D) / a;
    t1 = (-half_b + sqrt_D) / a;
    return true;
}

Collision Sphere::GetCollision(const Ray &ray, float t) {
    Collision c;
    c.pos = ray.origin + ray.direction*t;
    glm::vec3 out_normal = glm::normalize(c.pos - m_center);
    
    float cos_angle = glm::dot(ray.direction, out_normal);
    // if bouncing against interior of sphere
    // then out normal will radiate in same direction as ray direction
    // cos(theta) < 0 if theta > 90' and theta < -90'   (external collision)
    // cos(theta) > 0 if -90' < theta < 90'             (internal collision)
    c.is_internal = cos_angle > 0;
    c.normal = c.is_internal ? -out_normal : out_normal;
    return c;
}

}