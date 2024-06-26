#pragma once
#ifndef GAMES_3D_HPP
#define GAMES_3D_HPP

#include "mat.hpp"

namespace games
{

template <floating_point T>
auto rotation3d(const vec3<T> &axis, T angle) -> mat3x3<T>
{
    auto c = std::cos(angle);
    auto s = std::sin(angle);
    auto t = 1 - c;
    auto x = axis[0], y = axis[1], z = axis[2];
    // clang-format off
    return mat3x3<T>{t * x * x + c, t * x * y - s * z, t * x * z + s * y,
                     t * x * y + s * z, t * y * y + c, t * y * z - s * x,
                     t * x * z - s * y, t * y * z + s * x, t * z * z + c};
    // clang-format on
}

template <floating_point T>
class transform3d
{
  private:
    mat4x4<T> m_trans;

  public:
    transform3d() : m_trans(mat4x4<T>::identity()) {}
    transform3d(const mat4x4<T> &t) : m_trans(t) {}

    friend transform3d operator*(const transform3d &t1, const transform3d &t2) { return {t1.m_trans * t2.m_trans}; }

    template <arithmetic U>
    friend auto operator*(const transform3d &t, const vec4<U> &v)
    {
        return t.m_trans * v;
    }

    static transform3d identity() { return {mat4x4<T>::identity()}; }

    static transform3d translation(const vec3<T> &v)
    {
        return {mat4x4<T>{1, 0, 0, v[0], 0, 1, 0, v[1], 0, 0, 1, v[2], 0, 0, 0, 1}};
    }

    static transform3d rotation(const vec3<T> &axis, T angle)
    {
        auto c = std::cos(angle);
        auto s = std::sin(angle);
        auto t = 1 - c;
        auto x = axis[0], y = axis[1], z = axis[2];
        // clang-format off
        return {mat4x4<T>{t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
                          t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
                          t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
                          0, 0, 0, 1}};
        // clang-format on
    }

    static transform3d scaling(const vec3<T> &v)
    {
        return {mat4x4<T>{v[0], 0, 0, 0, 0, v[1], 0, 0, 0, 0, v[2], 0, 0, 0, 0, 1}};
    }

    static transform3d perspective(T fov, T aspect, T near, T far)
    {
        auto f = 1 / std::tan(fov / 2);
        auto d = near - far;
        // clang-format off
        return {mat4x4<T>{f / aspect, 0, 0, 0,
                          0, f, 0, 0,
                          0, 0, (far + near) / d, 2 * far * near / d,
                          0, 0, -1, 0}};
        // clang-format on
    }

    static transform3d rotation_x(T angle)
    {
        auto c = std::cos(angle);
        auto s = std::sin(angle);
        // clang-format off
        return {mat4x4<T>{1, 0, 0, 0,
                          0, c, -s, 0,
                          0, s, c, 0,
                          0, 0, 0, 1}};
        // clang-format on
    }

    static transform3d rotation_y(T angle)
    {
        auto c = std::cos(angle);
        auto s = std::sin(angle);
        // clang-format off
        return {mat4x4<T>{c, 0, s, 0,
                          0, 1, 0, 0,
                          -s, 0, c, 0,
                          0, 0, 0, 1}};
        // clang-format on
    }

    static transform3d rotation_z(T angle)
    {
        auto c = std::cos(angle);
        auto s = std::sin(angle);
        // clang-format off
        return {mat4x4<T>{c, -s, 0, 0,
                          s, c, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1}};
        // clang-format on
    }
};

} // end namespace games

#endif // GAMES_3D_HPP