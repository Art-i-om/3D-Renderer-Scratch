#pragma once

#include <cassert>
#include <iostream>
#include <cmath>

// #### FACE UNIT ####
struct FaceUnit {
    int v = 0, vt = 0, vn = 0;
    int& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? vt : vn) : v; }
    int operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? vt : vn) : v; }
};

inline std::ostream& operator<<(std::ostream& out, const FaceUnit& f) {
    for (int i = 0; i < 3; i++) {
        out << f[i];
        if (i != 2) out << "/";
    }
    out << " ";

    return out;
}
// #### FACE UNIT END ####

// #### FACE ####
struct Face {
    FaceUnit f1, f2, f3;
    FaceUnit& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? f2 : f3) : f1; }
    FaceUnit operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? f2 : f3) : f1; }
};

inline std::ostream& operator<<(std::ostream& out, const Face& f) {
    for (int i = 0; i < 3; i++) {
        out << f[i] << " ";
    }

    return out;
}
// #### FACE END ####

// #### VECTOR ####
template<int n> struct vec {
    double data[n] = {0};
    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
    double operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    double ret = 0;
    for (int i = n; i--; ret += lhs[i] * rhs[i]) {}
    return ret;
}

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] += rhs[i]) {}
    return ret;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] -= rhs[i]) {}
    return ret;
}

template<int n> vec<n> operator*(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs) {}
    return ret;
}

template<int n> vec<n> operator*(const double& lhs, const vec<n>& rhs) {
    return rhs * lhs;
}

template<int n> vec<n> operator/(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] /= rhs) {}
    return ret;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) {
        out << v[i] << " ";
    }

    return out;
}

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 4); return i<2 ? (i ? y : x) : (i == 2 ? z : w); }
    double operator[](const int i) const { assert(i >= 0 && i < 4); return i<2 ? (i ? y : x) : (i == 2 ? z : w); }
    [[nodiscard]] vec<2> xy() const { return {x, y}; }
    [[nodiscard]] vec<3> xyz() const { return {x, y, z}; }
};

typedef vec<2> Vec2;
typedef vec<3> Vec3;
typedef vec<4> Vec4;

template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v*v);
}

template<int n> vec<n> normalized(const vec<n>& v) {
    return v / norm(v);
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return {
        v1.y*v2.z - v1.z*v2.y,
        v1.z*v2.x - v1.x*v2.z,
        v1.x*v2.y - v1.y*v2.x
    };
}

// #### VECTOR END ####

// #### MATRIX ####
template<int n> struct dt;

template<int nrows, int ncols> struct Matrix {
    vec<ncols> rows[nrows] = {{}};

    vec<ncols>& operator[] (const int idx) { assert(idx >= 0 && idx < nrows); return rows[idx]; }
    const vec<ncols>& operator[] (const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }

    double det() const {
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        Matrix<nrows - 1, ncols - 1> submatrix;
        for (int i = nrows - 1; i--; )
            for (int j = ncols - 1; j--; submatrix[i][j] = rows[i + int(i >= row)][j + int(j >= col)]) {}

        return submatrix.det() * ((row + col) % 2 ? -1 : 1);
    }

    Matrix<nrows, ncols> invert_transpose() const {
        Matrix<nrows, ncols> adjugate_transpose;
        for (int i = nrows; i--; )
            for (int j = ncols; j--; adjugate_transpose[i][j] = cofactor(i, j)) {}

        return adjugate_transpose / (adjugate_transpose[0] * rows[0]);
    }

    Matrix<nrows, ncols> invert() const {
        return invert_transpose().transpose();
    }

    Matrix<ncols, nrows> transpose() const {
        Matrix<ncols, nrows> ret;
        for (int i = ncols; i--; )
            for (int j = nrows; j--; ret[i][j] = rows[j][i]) {}

        return ret;
    }
};

template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows>& lhs, const Matrix<nrows, ncols>& rhs) {
    return (Matrix<1, nrows>{{lhs}} * rhs)[0];
}

template<int nrows, int ncols> vec<nrows> operator*(const Matrix<nrows, ncols>& lhs, const vec<ncols>& rhs) {
    vec<nrows> ret;
    for (int i = nrows; i--; ret[i] = lhs[i] * rhs) {}
    return ret;
}

template<int R1, int C1, int C2> Matrix<R1, C2> operator*(const Matrix<R1, C1>& lhs, const Matrix<C1, C2>& rhs) {
    Matrix<R1, C2> result;
    for (int i = R1; i--; )
        for (int j = C2; j--; )
            for (int k = C1; k--; result[i][j] += lhs[i][k] * rhs[k][j]) {}

    return result;
}

template<int nrows, int ncols> Matrix<nrows, ncols> operator*(const Matrix<nrows, ncols>& lhs, const double& val) {
    Matrix<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] * val) {}
    return result;
}

template<int nrows, int ncols> Matrix<nrows, ncols> operator/(const Matrix<nrows, ncols>& lhs, const double& val) {
    Matrix<nrows, ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]/val) {}
    return result;
}

template<int nrows, int ncols> Matrix<nrows, ncols> operator+(const Matrix<nrows,ncols>& lhs, const Matrix<nrows,ncols>& rhs) {
    Matrix<nrows,ncols> result;
    for (int i=nrows; i--; )
        for (int j=ncols; j--; result[i][j]=lhs[i][j]+rhs[i][j]);
    return result;
}

template<int nrows, int ncols> Matrix<nrows, ncols> operator-(const Matrix<nrows,ncols>& lhs, const Matrix<nrows,ncols>& rhs) {
    Matrix<nrows,ncols> result;
    for (int i=nrows; i--; )
        for (int j=ncols; j--; result[i][j]=lhs[i][j]-rhs[i][j]) {}
    return result;
}

template<int nrows, int ncols> std::ostream& operator<<(std::ostream& out, const Matrix<nrows, ncols>& m) {
    for (int i = 0; i < nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct dt {
    static double det(const Matrix<n, n>& src) {
        double ret = 0;
        for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<> struct dt<1> {
    static double det(const Matrix<1, 1>& src) {
        return src[0][0];
    }
};

// #### MATRIX END ####