// headmodel/geom/Vec3.h
#pragma once
#include <cmath>

namespace headmodel::geom {
struct Vec3 {
    double x{}, y{}, z{};
    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s) const { return {x*s, y*s, z*s}; }
};
inline Vec3 lerp(const Vec3& a, const Vec3& b, double t) { return a + (b-a)*t; }
}

