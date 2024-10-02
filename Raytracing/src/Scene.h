#pragma once

#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"

struct ObjectProperties {
    point3 position;
    double radius;
    std::shared_ptr<material> mat_ptr;
};

struct Scene {
    std::vector<ObjectProperties> object_properties;
    std::vector<std::shared_ptr<material>> materials;

    hittable_list create_hittable_list() const {
        hittable_list world;
        for (const auto& prop : object_properties) {
            world.add(make_shared<sphere>(prop.position, prop.radius, prop.mat_ptr));
        }
        return hittable_list(make_shared<bvh_node>(world));
    }
};

Scene create_scene() {
    Scene scene;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    scene.materials.push_back(material_ground);
    scene.materials.push_back(material_center);
    scene.materials.push_back(material_left);
    scene.materials.push_back(material_bubble);
    scene.materials.push_back(material_right);

    scene.object_properties.push_back({ point3(0.0, -100.5, -1.0), 100.0, material_ground });
    scene.object_properties.push_back({ point3(0.0, 0.0, -1.2), 0.5, material_center });
    scene.object_properties.push_back({ point3(-1.0, 0.0, -1.0), 0.5, material_left });
    scene.object_properties.push_back({ point3(-1.0, 0.0, -1.0), 0.4, material_bubble });
    scene.object_properties.push_back({ point3(1.0, 0.0, -1.0), 0.5, material_right });

    return scene;
}
