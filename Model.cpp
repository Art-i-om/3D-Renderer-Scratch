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
            facet_nrm.push_back(v);
        }
        else if (prefix == "f") {
            char slash1, slash2;
            Face f;
            facet_vrt.push_back({});
            for (int i = 0; i < 3; i++) {
                ss >> f[i][0] >> slash1 >> f[i][1] >> slash2 >> f[i][2];
                f[i][0]--; f[i][1]--; f[i][2]--;
            }

            facet_vrt.push_back(f);
        }
    }

     model.close();
}

Vec3 Model::vert(int i) {
    return vertices[i];
}

Vec3 Model::vert(int iface, int nvert) const {
    if (nvert < 0 || nvert >= 3) {
        std::cerr << "Index out of range in Model::vert()\n";
        return {0, 0, 0};
    }
    return vertices[facet_vrt[iface][nvert].v];
}

Vec3 Model::normal(const int &iface, const int &ivert) const {
    return facet_nrm[facet_vrt[iface][ivert].vn];
}
