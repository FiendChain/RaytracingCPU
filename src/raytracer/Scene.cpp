#include "Scene.h"
#include <limits>

namespace raytracer {

Scene::Scene()
:   m_dielectric(), m_lambertian(), m_metal(),
    m_spheres(),
    m_entities(), m_intersection_entities(), m_difference_entities()
{}

Scene::CastResult Scene::CastRay(Ray &ray) {
    float t_min = 0.001f;
    float t_closest = std::numeric_limits<float>::infinity();

    IShape *shape = nullptr;
    IMaterial *material = nullptr;

    for (auto entity: m_entities) {
        RayCast cast;
        // if missed the entity
        if (!entity->CastRay(ray, cast)) {
            continue;
        }

        // check to see if interval is closer
        float t = cast.t0;
        if (t < t_min || t > t_closest) {
            t = cast.t1;
            if (t < t_min || t > t_closest) {
               continue; 
            } 
        }

        // if it is closer, then use it
        t_closest = t; 
        shape = cast.shape;
        material = cast.material;
    }

    // if no object was found
    if (!shape) {
       return {false, false}; 
    }
    
    // find the collision against the closest entity hit by ray
    Collision collision = shape->GetCollision(ray, t_closest); 
    bool has_scatter = material->CastRay(ray, collision);
    return {true, has_scatter};
}

}