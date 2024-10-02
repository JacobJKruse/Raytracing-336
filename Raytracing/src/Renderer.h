#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include "rtweekend.h"

#include "Camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

class Renderer
{
public:
    void Render(const Camera& camera, const hittable& world);
    void OnResize(uint32_t width, uint32_t height);
    std::shared_ptr<Walnut::Image> GetFinalImage() const { return FinalImg; };



private:
    uint32_t TraceRay(const ray& ray, const hittable& world);
    
    // Color scale factor for a sum of pixel samples
    std::shared_ptr<Walnut::Image> FinalImg;
    uint32_t* imgData = nullptr;
    point3 center;               // Camera center
    point3 pixel00_loc;          // Location of pixel 0, 0
    vec3   pixel_delta_u;        // Offset to pixel to the right
    vec3   pixel_delta_v;        // Offset to pixel below
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    int samples_per_pixel = 10;
    double pixel_samples_scale = 1.0 / samples_per_pixel;
};

