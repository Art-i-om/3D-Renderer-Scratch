#pragma once

#include "tgaimage.h"
#include "geometry.h"

void lookat(const Vec3& eye, const Vec3& center, const Vec3& up);
void init_perspective(const double& f);
void init_viewport(const int& x, const int& y, const int& w, const int& h);
void init_zbuffer(const int& width, const int& height);

struct IShader {
    virtual ~IShader() = default;

    static TGAColor sample2D(const TGAImage& img, const Vec2& uvf) {
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }

    [[nodiscard]] virtual std::pair<bool, TGAColor> fragment(const Vec3& bar) const = 0;
};

typedef Vec4 Triangle[3];
void rasterize(const Triangle& clip, const IShader& shader, TGAImage& framebuffer);