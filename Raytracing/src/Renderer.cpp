#include "Renderer.h"
#include "Walnut/Random.h"


double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (h - std::sqrt(discriminant)) / a;
    }
}

color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}



vec3 sample_square() {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

void Renderer::Render(const Camera& camera) {
    glm::vec3 cameraPosition = camera.GetPosition();

    // Convert glm::vec3 to your vec3 type
    vec3 myCameraPosition(cameraPosition);

    for (uint32_t y = 0; y < FinalImg->GetHeight(); y++) {
        for (uint32_t x = 0; x < FinalImg->GetWidth(); x++) {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
                vec3 offset = sample_square() * 0.001; // Further reduce the offset scale
                const glm::vec3& rayDir = camera.GetRayDirections()[x + y * FinalImg->GetWidth()];
               // vec3 myRayDir = unit_vector(rayDir); // Normalize the direction

                vec3 myRayDir = unit_vector(rayDir + offset); // Normalize the direction
                ray cameraRay(myCameraPosition, myRayDir);
                pixel_color += ray_color(cameraRay);
            }

            // Average the color and apply gamma correction
            pixel_color /= samples_per_pixel;
            // = color(sqrt(pixel_color.x()), sqrt(pixel_color.y()), sqrt(pixel_color.z()));

            imgData[x + y * FinalImg->GetWidth()] = write_color(pixel_color);
        }
    }
    FinalImg->SetData(imgData);
}



uint32_t Renderer::TraceRay(const ray& ray) {
    // Get the color from the ray
    color pixel_color = ray_color(ray);

    // Convert the color to RGBA format
    return write_color(pixel_color);
}

void Renderer::OnResize(uint32_t width, uint32_t height) {
    if (FinalImg) {
        if (width == FinalImg->GetWidth() && height == FinalImg->GetHeight()) {
            return;
        }
        FinalImg->Resize(width, height);
    }
    else {
        FinalImg = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] imgData;
    imgData = new uint32_t[width * height];
}


