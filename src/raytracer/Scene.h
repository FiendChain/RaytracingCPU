#pragma once

#include "Shape.h"
#include "Material.h"
#include "Ray.h"
#include "Entity.h"

#include <vector>

namespace raytracer {

// A scene is responsible for:
// - storing all the entity data
// - updating the ray's color as it bounces througout the scene
class Scene {
    public:
        // Result of a ray cast, and indicate the termination condition
        struct CastResult {
            public:
                bool hit_object;
                bool no_bounce;
        };
    public:
        // shapes
        std::vector<Sphere> m_spheres;
        // materials
        std::vector<Lambertian> m_lambertian;
        std::vector<Dielectric> m_dielectric;
        std::vector<Metal> m_metal;
        // entities
        std::vector<IEntity*> m_entities;
        std::vector<BasicEntity> m_basic_entities;
        std::vector<IntersectionEntity> m_intersection_entities;
        std::vector<DifferenceEntity> m_difference_entities;
    public:
        Scene();
        CastResult CastRay(Ray& ray);
};

}