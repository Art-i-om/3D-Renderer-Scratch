#pragma once

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec4> vertices = {};
    std::vector<Vec4> norms = {};
    std::vector<Vec2> tex = {};

    std::vector<Face> facet_vrt = {};

    TGAImage normal_map = {};
    TGAImage diffuse_map = {};
    TGAImage specular_map = {};

public:
    Model(const std::string& filename);
    [[nodiscard]] int nverts() const { return static_cast<int>(vertices.size()); }
    [[nodiscard]] int nfaces() const { return static_cast<int>(facet_vrt.size()); }
    
    [[nodiscard]] Vec4 vert(const int& i) const;
    [[nodiscard]] Vec4 vert(int iface, int nvert) const;
    [[nodiscard]] Vec4 normal(const int& iface, const int& ivert) const;
    [[nodiscard]] Vec4 normal(const Vec2& uv) const;
    [[nodiscard]] Vec2 uv(const int& iface, const int& nthvert) const;

    const TGAImage& diffuse() const;
    const TGAImage& specular() const;
};
