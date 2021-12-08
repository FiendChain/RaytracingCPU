#include "Entity.h"

namespace raytracer {

bool BasicEntity::CastRay(const Ray &ray, RayCast &cast) {
    float t0, t1;
    if (!m_shape->CheckHit(ray, t0, t1)) {
        return false;
    }

    cast.t0 = t0;
    cast.t1 = t1;
    cast.shape = m_shape;
    cast.material = m_material;
    return true;
}

bool IntersectionEntity::CastRay(const Ray &ray, RayCast &cast) {
    RayCast left_cast, right_cast;

    bool left_cast_success = m_left->CastRay(ray, left_cast);
    bool right_cast_success = m_right->CastRay(ray, right_cast);

    if (!left_cast_success || !right_cast_success) {
        return false;
    }

    float t0 = glm::max(left_cast.t0, right_cast.t0);
    float t1 = glm::min(left_cast.t1, right_cast.t1);
    if (t0 > t1) {
        return false;
    }

    cast.t0 = t0;
    cast.t1 = t1;

    bool hit_left = left_cast.t0 > right_cast.t0;

    cast.material = hit_left ? left_cast.material : right_cast.material;
    cast.shape = hit_left ? left_cast.shape : right_cast.shape;
    return true;
}

bool DifferenceEntity::CastRay(const Ray &ray, RayCast &cast) {
    RayCast left_cast, right_cast;

    bool left_cast_success = m_left->CastRay(ray, left_cast);
    bool right_cast_success = m_right->CastRay(ray, right_cast);

    if (!left_cast_success) {
        return false;
    }

    // don't subtract
    if (!right_cast_success) {
        cast = left_cast;
        return true;
    }

    // difference is disjoint, don't substract
    if (right_cast.t0 > left_cast.t1 || left_cast.t0 > right_cast.t1) {
        cast = left_cast;
        return true;
    }

    // difference covers entire original entity
    if (right_cast.t1 > left_cast.t1 && right_cast.t0 < left_cast.t0) {
        return false;
    }

    // difference is behind
    if (right_cast.t0 > left_cast.t0) {
        cast.t0 = left_cast.t0;
        cast.t1 = glm::min(left_cast.t1, right_cast.t0);
        cast.material = left_cast.material;
        cast.shape = left_cast.shape;
        return true;
    }


    // difference is in front
    cast.t0 = right_cast.t1;
    cast.t1 = left_cast.t1;
    cast.material = right_cast.material;
    cast.shape = right_cast.shape;
    return true;
}

}