#include "our_gl.h"
#include <algorithm>

Matrix<4, 4> ModelView, Viewport, Perspective;
std::vector<double> zbuffer;

void lookat(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
    const Vec3 n = normalized(eye - center);
    const Vec3 l = normalized(cross(up, n));
    const Vec3 m = normalized(cross(n, l));
    ModelView =
        Matrix<4, 4>{
            {
                {l.x, l.y, l.z, 0},
                {m.x, m.y, m.z, 0},
                {n.x, n.y, n.z, 0},
                {0, 0, 0, 1}
            }}
    *
    Matrix<4, 4>{
                {
                    {1, 0, 0, -center.x},
                    {0, 1, 0, -center.y},
                    {0, 0, 1, -center.z},
                    {0, 0, 0, 1}
                }};
}

void init_perspective(const double &f) {
    Perspective = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, -1 / f, 1}
    }};
}

void init_viewport(const int &x, const int &y, const int &w, const int &h) {
    Viewport = {
        {
            {w / 2.0, 0, 0, x + w / 2.0},
            {0, h / 2.0, 0, y + h / 2.0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }
    };
}

void init_zbuffer(const int &width, const int &height) {
    zbuffer = std::vector(width*height, -1000.0);
}

void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer) {
    Vec4 ndc[3] = {clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w};
    Vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport*ndc[1]).xy(), (Viewport*ndc[2]).xy() };

    Matrix<3, 3> ABC = {{
        {screen[0].x, screen[0].y, 1.0},
        {screen[1].x, screen[1].y, 1.0},
        {screen[2].x, screen[2].y, 1.0 }
    }};
    if (ABC.det() < 1) return;

    auto bb_min_x = std::min({screen[0].x, screen[1].x, screen[2].x});
    auto bb_max_x = std::max({screen[0].x, screen[1].x, screen[2].x});

    auto bb_min_y = std::min({screen[0].y, screen[1].y, screen[2].y});
    auto bb_max_y = std::max({screen[0].y, screen[1].y, screen[2].y});

#pragma omp parallel for
    for (int x = std::max<int>(bb_min_x, 0); x <= std::min<int>(bb_max_x, framebuffer.width() - 1); x++) {
        for (int y = std::max<int>(bb_min_y, 0); y <= std::min<int>(bb_max_y, framebuffer.width() - 1); y++) {
            Vec3 bc = ABC.invert_transpose() * Vec3(static_cast<double>(x), static_cast<double>(y), 1.0);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            double z = bc * Vec3{ ndc[0].z, ndc[1].z, ndc[2].z };
            if (z <= zbuffer[x + y * framebuffer.width()]) continue;
            auto [discard, color] = shader.fragment(bc);
            if (discard) continue;
            zbuffer[x + y * framebuffer.width()] = z;
            framebuffer.set(x, y, color);
        }
    }
}
