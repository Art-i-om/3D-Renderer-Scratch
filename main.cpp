#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>

#include "Model.h"
#include "our_gl.h"
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

extern Matrix<4, 4> ModelView, Perspective;
extern std::vector<double> zbuffer;

std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<int> dist(0, 255);

struct PhongShader : IShader {
    const Model& model;
    Vec4 l;
    Vec2 varying_uv[3];

    PhongShader(const Vec3 light, const Model& m) : model(m) {
        l = normalized(ModelView * Vec4{light.x, light.y, light.z, 0.});
    }

    virtual Vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face, vert);
        Vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const Vec3 &bar) const {
        // Vec3 n = normalized(cross(tri[1] - tri[0], tri[2] - tri[0]));
        Vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        Vec4 n = normalized(ModelView.invert_transpose() * model.normal(uv));
        Vec4 r = normalized(n * (n * l)*2 - l);
        double ambient = .4;
        double diff = 1.*std::max(0., n * l);
        double spec = (3.*sample2D(model.specular(), uv)[0] / 255.) * std::pow(std::max(r.z, 0.), 35);

        TGAColor gl_FragColor = sample2D(model.diffuse(), uv);

        for (int channel : {0, 1, 2}) {
            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel] * (ambient + diff + spec));
        }

        return {false, gl_FragColor};
    }
};

int main(int argc, char** argv) {
    constexpr int width = 800;
    constexpr int height = 800;

    constexpr Vec3 light = {0, 0, 1};
    constexpr Vec3 eye = {-1, 0, 2};
    constexpr Vec3 center = {0, 0, 0};
    constexpr Vec3 up = {0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(width / 16, height / 16, width * 7/8, height * 7/8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model("../africa_head.obj");
    PhongShader shader(light, model);

    for (int f = 0; f < model.nfaces(); f++) {
        Triangle clip = {
            shader.vertex(f, 0),
            shader.vertex(f, 1),
            shader.vertex(f, 2)
        };

        rasterize(clip, shader, framebuffer);
    }

    bool ok = framebuffer.write_tga_file("framebuffer.tga");
    if (!ok) {
        std::cout << "Cannot write to .tga file.\n";
    }

    return 0;
}