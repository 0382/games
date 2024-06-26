#pragma once
#ifndef GAMES_2D_HPP
#define GAMES_2D_HPP

#include "mat.hpp"

namespace games
{

template <arithmetic T>
auto rotation2d(T angle) -> mat2x2<decltype(std::cos(angle))>
{
    auto c = std::cos(angle);
    auto s = std::sin(angle);
    return mat2x2<T>{c, -s, s, c};
}

template <floating_point T>
vec2<T> rotate(const vec2<T> &v, arithmetic auto angle)
{
    auto c = std::cos(angle);
    auto s = std::sin(angle);
    return vec2<T>{v[0] * c - v[1] * s, v[0] * s + v[1] * c};
}

template <arithmetic T>
auto scaling2d(const vec2<T> &v) -> mat2x2<T>
{
    return mat2x2<T>{v[0], 0, 0, v[1]};
}

template <floating_point T>
class transform2d
{
  private:
    mat2x2<T> m_rotation;
    vec2<T> m_translation;

  public:
    transform2d() : m_rotation(mat2x2<T>::identity()), m_translation(vec2<T>::zero()) {}
    transform2d(const mat2x2<T> &r, const vec2<T> &t) : m_rotation(r), m_translation(t) {}

    friend transform2d operator*(const transform2d &t1, const transform2d &t2)
    {
        return {t1.m_rotation * t2.m_rotation, t1.m_translation + t1.m_rotation * t2.m_translation};
    }

    template <arithmetic U>
    friend auto operator*(const transform2d &t, const vec2<U> &v)
    {
        return t.m_translation + t.m_rotation * v;
    }

    static transform2d identity() { return {mat2x2<T>::identity(), vec2<T>::zero()}; }
    static transform2d translation(const vec2<T> &v) { return {mat2x2<T>::identity(), v}; }
    static transform2d rotation(T angle) { return {rotation2d(angle), vec2<T>::zero()}; }
    static transform2d scaling(const vec2<T> &v) { return {scaling2d(v), vec2<T>::zero()}; }
};

} // end namespace games

#endif // GAMES_2D_HPP