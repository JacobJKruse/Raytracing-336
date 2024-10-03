#include <vector>
#include <thread>
#include <future>
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

color ray_color(const ray& r, int depth, const hittable& world) {
    if (depth <= 0)
        return color(0, 0, 0);

    hit_record rec;

    if (world.hit(r, interval(0.001, infinity), rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, depth - 1, world);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

vec3 sample_square() {
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

void Renderer::render_tile(Renderer& renderer, const Camera& camera, const hittable& world, uint32_t start_y, uint32_t end_y) {
    glm::vec3 cameraPosition = camera.GetPosition();
    vec3 myCameraPosition(cameraPosition);

    for (uint32_t y = start_y; y < end_y; y++) {
        for (uint32_t x = 0; x < renderer.FinalImg->GetWidth(); x++) {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < renderer.samples_per_pixel; sample++) {
                vec3 offset = sample_square() * 0.001;
                const glm::vec3& rayDir = camera.GetRayDirections()[x + y * renderer.FinalImg->GetWidth()];
                vec3 myRayDir = unit_vector(rayDir + offset);
                ray cameraRay(myCameraPosition, myRayDir);
                pixel_color += ray_color(cameraRay, renderer.max_depth, world);
            }

            pixel_color /= renderer.samples_per_pixel;
            renderer.imgData[x + y * renderer.FinalImg->GetWidth()] = write_color(pixel_color);
        }
    }
}

void Renderer::Render(const Camera& camera, const hittable& world) {
    uint32_t height = FinalImg->GetHeight();
    uint32_t num_threads = std::thread::hardware_concurrency();
    uint32_t rows_per_thread = height / num_threads;

    std::vector<std::thread> threads;

    for (uint32_t i = 0; i < num_threads; ++i) {
        uint32_t start_y = i * rows_per_thread;
        uint32_t end_y = (i == num_threads - 1) ? height : start_y + rows_per_thread;
        threads.emplace_back(&Renderer::render_tile, this, std::ref(*this), std::ref(camera), std::ref(world), start_y, end_y);
    }


    for (auto& thread : threads) {
        thread.join();
    }

    FinalImg->SetData(imgData);
}

uint32_t Renderer::TraceRay(const ray& ray, const hittable& world) {
    color pixel_color = ray_color(ray, max_depth, world);
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
