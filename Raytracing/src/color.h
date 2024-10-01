#pragma once
#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "rtweekend.h"
#include "interval.h"
namespace {
    using color = vec3;

    inline double linear_to_gamma(double linear_component)
    {
        if (linear_component > 0)
            return std::sqrt(linear_component);

        return 0;
    }

    uint32_t write_color(const color& pixel_color) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Apply a linear to gamma transform for gamma 2
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Translate the [0,1] component values to the byte range [0,255].
        static const interval intensity(0.000, 0.999);
        uint8_t rbyte = static_cast<uint8_t>(255.999 * intensity.clamp(r));
        uint8_t gbyte = static_cast<uint8_t>(255.999 * intensity.clamp(g));
        uint8_t bbyte = static_cast<uint8_t>(255.999 * intensity.clamp(b));
        uint8_t abyte = static_cast<uint8_t>(255.999 * 1.0); // Assuming alpha is 1.0

        // Combine the color components into a single 0x______ value.
        return (abyte << 24) | (bbyte << 16) | (gbyte << 8) | rbyte;
    }
}
#endif