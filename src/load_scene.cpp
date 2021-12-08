#include "load_scene.h"

#include <glm/glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <random>

// Create a bunch of small balls
void load_small_balls(raytracer::Scene &scene, std::mt19937 &rng);

// Load entities into scene
void load_scene(raytracer::Scene &scene)
{
    // TODO: Replace this with a memory pool system
    // Right now there is no way to enforce the vector from resizing and changing the location of the objects
    // If a vector resizes, the entity vector will point to material/shapes/sub_entities in the wrong location causing an illegal memory access
    scene.m_dielectric.reserve(600);
    scene.m_lambertian.reserve(600);
    scene.m_metal.reserve(600);
    scene.m_spheres.reserve(600);
    scene.m_entities.reserve(600);
    scene.m_basic_entities.reserve(600);
    scene.m_intersection_entities.reserve(50);
    scene.m_difference_entities.reserve(50);

    // Scene layout based on: https://github.com/valerioformato/RTIAW
    std::mt19937 rng{};
    #if 1
    load_small_balls(scene, rng);
    #endif

    // Reflective metal ball
    {
        // auto& material = scene.m_lambertian.emplace_back(glm::vec3{0.3, 0.3, 0.3});
        auto& material = scene.m_metal.emplace_back(glm::vec3(0.4, 0.4, 0.4), 0.1f);
        auto& shape = scene.m_spheres.emplace_back(glm::vec3{0,-5000,0}, 5000.0f);
        auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
        scene.m_entities.push_back(&entity);
    }

    // Glass ball
    {
        auto& material = scene.m_dielectric.emplace_back(1.5f);
        auto& shape = scene.m_spheres.emplace_back(glm::vec3{0, 1, 0}, 1.0f);
        auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
        scene.m_entities.push_back(&entity);
    }

    // Diffuse ball
    {
        auto& material = scene.m_lambertian.emplace_back(glm::vec3(0.4, 0.2, 0.1));
        auto& shape = scene.m_spheres.emplace_back(glm::vec3{-4, 1, 0}, 1.0f);
        auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
        scene.m_entities.push_back(&entity);
    }

    // Half mirror and half diffuse ball using intersection
    // Shape is intersect of two spheres
    {
        auto& left_material = scene.m_metal.emplace_back(glm::vec3(0.7, 0.6, 0.5), 0.0f);
        auto& right_material = scene.m_lambertian.emplace_back(glm::vec3(0.4, 0.6, 0.1));
        // auto& left_material = scene.m_dielectric.emplace_back(1.5f);
        // auto& right_material = left_material;

        auto& left_shape = scene.m_spheres.emplace_back(glm::vec3{4, 2, 3}, 1.0f);
        auto& right_shape = scene.m_spheres.emplace_back(glm::vec3{4, 2, 3.3}, 1.0f);

        auto& left_entity = scene.m_basic_entities.emplace_back(&left_shape, &left_material);
        auto& right_entity = scene.m_basic_entities.emplace_back(&right_shape, &right_material);

        auto& entity = scene.m_intersection_entities.emplace_back(&left_entity, &right_entity);
        scene.m_entities.push_back(&entity);
    }

    // Spherical mirror with matte backing
    {
        auto& left_material = scene.m_lambertian.emplace_back(glm::vec3(0.8, 0.2, 0.2));
        // auto& right_material = scene.m_lambertian.emplace_back(glm::vec3(0.4, 0.6, 0.1));
        // auto& left_material = scene.m_dielectric.emplace_back(1.5f);
        // auto& right_material = left_material;
        // auto& left_material = scene.m_metal.emplace_back(glm::vec3(0.4, 0.8, 0.9), 0.0f);
        auto& right_material = scene.m_metal.emplace_back(glm::vec3(0.7, 0.7, 0.7), 0.0f);

        auto& left_shape = scene.m_spheres.emplace_back(glm::vec3{4, 1, -1}, 1.0f);
        auto& right_shape = scene.m_spheres.emplace_back(glm::vec3{4.1, 0.8, -1.0}, 1.0f);

        auto& left_entity = scene.m_basic_entities.emplace_back(&left_shape, &left_material);
        auto& right_entity = scene.m_basic_entities.emplace_back(&right_shape, &right_material);

        auto& entity = scene.m_difference_entities.emplace_back(&left_entity, &right_entity);
        scene.m_entities.push_back(&entity);
    }

    // Glass biconvex lens
    {
        auto& left_material = scene.m_dielectric.emplace_back(1.3f, glm::vec3{0.6, 0.6, 0.6});
        auto& right_material = left_material;

        auto& left_shape = scene.m_spheres.emplace_back(glm::vec3{7, 0.6, 1}, 0.5f);
        auto& right_shape = scene.m_spheres.emplace_back(glm::vec3{7.2, 0.65, 1}, 0.5f);

        auto& left_entity = scene.m_basic_entities.emplace_back(&left_shape, &left_material);
        auto& right_entity = scene.m_basic_entities.emplace_back(&right_shape, &right_material);

        auto& entity = scene.m_intersection_entities.emplace_back(&left_entity, &right_entity);
        scene.m_entities.push_back(&entity);
    }

    // Create a golf ball 
    // Dimples in ball have a different material to golf ball
    {
        auto& ball_material = scene.m_metal.emplace_back(glm::vec3(0.3, 0.3, 0.3), 0.3f);
        // auto& ball_material = scene.m_dielectric.emplace_back(1.7f);
        auto& hole_material = scene.m_metal.emplace_back(glm::vec3(0.9, 0.9, 0.9), 0.04f);
        // auto& hole_material = scene.m_lambertian.emplace_back(glm::vec3(0.8, 0.2, 0.2));

        glm::vec3 ball_pos = glm::vec3{6.2, 0.65, 2};
        float ball_radius = 0.6f;
        auto& ball_shape = scene.m_spheres.emplace_back(ball_pos, ball_radius);

        auto& ball_entity = scene.m_basic_entities.emplace_back(&ball_shape, &ball_material);
        raytracer::IEntity* root_entity = &ball_entity;
        

        float hole_radius = 0.15f;

        for (int i = 0; i < 20; i++) {
            glm::vec3 hole_pos = ball_pos + (ball_radius-0.05f)*glm::sphericalRand(1.0f);

            auto& hole_shape = scene.m_spheres.emplace_back(hole_pos, hole_radius);
            auto& hole_entity = scene.m_basic_entities.emplace_back(&hole_shape, &hole_material);

            auto& entity = scene.m_difference_entities.emplace_back(root_entity, &hole_entity);
            root_entity = &entity;
        }

        scene.m_entities.push_back(root_entity);
    }
}

// Create a bunch of small balls
void load_small_balls(raytracer::Scene &scene, std::mt19937 &rng)
{
    std::uniform_real_distribution<float> rand_float{0.0f, 1.0f};

    // layout balls in a grid around the origin
    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
        const auto choose_mat = rand_float(rng);
        glm::vec3 center(a + 0.9 * rand_float(rng), 0.2, b + 0.9 * rand_float(rng));

        if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
          if (choose_mat < 0.6f) 
          {
            // diffuse
            glm::vec3 randColor{rand_float(rng), rand_float(rng),
                            rand_float(rng)};
            auto albedo = randColor * randColor;

            auto& material = scene.m_lambertian.emplace_back(albedo);
            auto& shape = scene.m_spheres.emplace_back(center, 0.2f);
            auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
            scene.m_entities.push_back(&entity);
          } 
          else if (choose_mat < 0.9f) 
          {
            // metal
            glm::vec3 albedo{0.5f * (1.0f + rand_float(rng)),
                         0.5f * (1.0f + rand_float(rng)),
                         0.5f * (1.0f + rand_float(rng))};
            auto fuzz = 0.5f * rand_float(rng);

            auto& material = scene.m_metal.emplace_back(albedo, fuzz);
            auto& shape = scene.m_spheres.emplace_back(center, 0.2f);
            auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
            scene.m_entities.push_back(&entity);
          } 
          else 
          {
            // glass
            auto& material = scene.m_dielectric.emplace_back(1.5f);
            auto& shape = scene.m_spheres.emplace_back(center, 0.2f);
            auto& entity = scene.m_basic_entities.emplace_back(&shape, &material);
            scene.m_entities.push_back(&entity);
          }
        }
      }
    }
}