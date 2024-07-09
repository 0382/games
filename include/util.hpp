#pragma once
#ifndef GAMES_UTIL_HPP
#define GAMES_UTIL_HPP

#include <complex>
#include <concepts>

namespace games
{

template <typename T>
concept integral = std::integral<T>;

template <typename T>
concept floating_point = std::floating_point<T>;

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
constexpr inline bool is_complex_v = false;

template <arithmetic T>
constexpr inline bool is_complex_v<std::complex<T>> = true;

template <typename T>
concept complex = is_complex_v<T>;

template <typename T>
concept scalar = arithmetic<T> || complex<T>;

auto deg2rad(arithmetic auto deg) { return deg * 3.14159265358979323846 / 180.0; }
auto rad2deg(arithmetic auto rad) { return rad * 180.0 / 3.14159265358979323846; }

template <arithmetic T>
constexpr auto normalize_deg(T deg)
{
    while (deg >= 180)
        deg -= 360;
    while (deg < -180)
        deg += 360;
    return deg;
}

template <arithmetic T>
constexpr auto normalize_rad(T rad)
{
    while (rad >= 3.14159265358979323846)
        rad -= 2 * 3.14159265358979323846;
    while (rad < -3.14159265358979323846)
        rad += 2 * 3.14159265358979323846;
    return rad;
}

// normalize angle for geometry with Cn symmetry
template <arithmetic T>
constexpr auto normalize_rad_Cn(T angle, integral auto n)
{
    auto na = normalize_rad(angle);
    auto half = 3.14159265358979323846 / n;
    while (na >= half)
        na -= 2 * half;
    while (na < -half)
        na += 2 * half;
    return na;
}

template <floating_point T>
T lerp(T a, T b, T t)
{
    return a + (b - a) * t;
}

namespace literals
{

constexpr auto operator"" _deg(long double deg) { return deg2rad(deg); }

} // end namespace literals

} // end namespace games

#endif // GAMES_UTIL_HPP