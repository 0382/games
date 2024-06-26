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

namespace literals
{

constexpr auto operator"" _deg(long double deg) { return deg2rad(deg); }

} // end namespace literals

} // end namespace games

#endif // GAMES_UTIL_HPP