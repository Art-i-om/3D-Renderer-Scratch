#include "Model.h"
#include <fstream>
#include <sstream>

Model::Model(const std::string& filename) {
    std::ifstream model(filename);

    if (!model.is_open()) {
        std::cerr << "can't open file" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(model, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec4 v = {0, 0, 0, 1};
            ss >> v[0] >> v[1] >> v[2];
            vertices.push_back(v);
        }
        else if (prefix == "vn") {
            Vec4 v;
            ss >> v[0] >> v[1] >> v[2];
            norms.push_back(normalized(v));
        }
        else if (prefix == "vt") {
            Vec2 uv;
            ss >> uv[0] >> uv[1];
            tex.push_back({uv.x, 1 - uv.y});
        }
        else if (prefix == "f") {
            char slash1, slash2;
            Face f;
            for (int i = 0; i < 3; i++) {
                ss >> f[i][0] >> slash1 >> f[i][1] >> slash2 >> f[i][2];
                f[i][0]--; f[i][1]--; f[i][2]--;
            }

            facet_vrt.push_back(f);
        }
    }

    auto load_texture = [&filename](const std::string suffix, TGAImage& img) {
        size_t dot = filename.find_last_of(".");
        if (dot == std::string::npos) return;
        std::string texfile = filename.substr(0, dot) + suffix;
        std::cerr << "texture file " <<
            texfile << " loading " <<
                (img.read_tga_file(texfile.c_str()) ? "ok" : "failed")
        << std::endl;
    };
    load_texture("_diffuse.tga", diffuse_map);
    load_texture("_nm.tga", normal_map);
    load_texture("_spec.tga", specular_map);

     model.close();
}

Vec4 Model::vert(const int& i) const {
    return vertices[i];
}

Vec4 Model::vert(int iface, int nvert) const {
    if (nvert < 0 || nvert >= 3) {
        std::cerr << "Index out of range in Model::vert()\n";
        return {0, 0, 0};
    }
    return vertices[facet_vrt[iface][nvert].v];
}

Vec4 Model::normal(const int &iface, const int &ivert) const {
    if (ivert < 0 || ivert >= 3) {
        std::cerr << "Index out of range in Model::vert()\n";
        return {0, 0, 0};
    }
    return norms[facet_vrt[iface][ivert].vn];
}

Vec4 Model::normal(const Vec2 &uv) const {
    TGAColor c = normal_map.get(uv[0] * normal_map.width(), uv[1] * normal_map.height());
    return Vec4{static_cast<double>(c[2]),
        static_cast<double>(c[1]),
        static_cast<double>(c[0]),
        0} * 2./255. - Vec4{1, 1, 1, 0};
}

Vec2 Model::uv(const int &iface, const int &nthvert) const {
    if (nthvert < 0 || nthvert >= 3) {
        std::cerr << "Index out of range in Model::vert()\n";
        return {0, 0};
    }
    return tex[facet_vrt[iface][nthvert].vt];
}

const TGAImage &Model::diffuse() const {
    return diffuse_map;
}

const TGAImage &Model::specular() const {
    return specular_map;
}
