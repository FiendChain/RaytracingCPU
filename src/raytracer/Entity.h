#pragma once

#include "Ray.h"
#include "Shape.h"
#include "Material.h"

namespace raytracer
{

// what shape and material did the raycast hit
// what is the interval of the intersection
struct RayCast {
    public:
        IShape *shape;
        IMaterial *material;
        float t0, t1;
};

// An entity can take in a ray, and return via params whether it hit anything
class IEntity
{
    public:
        virtual bool CastRay(const Ray &ray, RayCast &cast) = 0;
};

class BasicEntity: public IEntity {
    public:
        BasicEntity(IShape* shape, IMaterial* material)
        : m_shape(shape), m_material(material) {}
        virtual bool CastRay(const Ray &ray, RayCast &cast);
    private:
        IShape* m_shape;
        IMaterial* m_material;
};

// For handling entity compositing
class ICompositeEntity: public IEntity {
    public:
        ICompositeEntity(IEntity* left, IEntity* right)
        : m_left(left), m_right(right) {}
        virtual bool CastRay(const Ray &ray, RayCast &cast) = 0;
    protected:
        IEntity* m_left;
        IEntity* m_right;
};

// Create a new entity that is an intersection of two different entities
class IntersectionEntity: public ICompositeEntity {
    public:
        IntersectionEntity(IEntity* left, IEntity* right)
        : ICompositeEntity(left, right) {}
        virtual bool CastRay(const Ray &ray, RayCast &cast);
};

// Create a new entity that is the first entity subtracted by the second entity
class DifferenceEntity: public ICompositeEntity {
    public:
        DifferenceEntity(IEntity* left, IEntity* right)
        : ICompositeEntity(left, right) {}
        virtual bool CastRay(const Ray &ray, RayCast &cast);
};

}