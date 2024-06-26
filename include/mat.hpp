#pragma once
#ifndef GAMES_MAT_HPP
#define GAMES_MAT_HPP

#include "util.hpp"
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>

namespace games
{

template <scalar T, std::size_t M, std::size_t N>
struct fixed_mat
{
    std::array<T, M * N> data;

    fixed_mat() = default;
    fixed_mat(const fixed_mat &m) = default;
    fixed_mat(fixed_mat &&m) = default;
    fixed_mat &operator=(const fixed_mat &m) = default;
    fixed_mat &operator=(fixed_mat &&m) = default;

    template <scalar... Args>
    fixed_mat(Args... args) : data{static_cast<T>(args)...}
    {}

    template <scalar U>
    explicit fixed_mat(const fixed_mat<U, M, N> &m)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] = static_cast<T>(m.data[i]);
        }
    }

    template <scalar U>
    fixed_mat &operator=(const fixed_mat<U, M, N> &m)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] = static_cast<T>(m.data[i]);
        }
        return *this;
    }

    template <scalar U>
    fixed_mat &operator=(U value)
    {
        static_assert(M == N, "M must be equal to N");
        data.fill(0);
        for (std::size_t i = 0; i < M; ++i)
        {
            data[i * N + i] = static_cast<T>(value);
        }
        return *this;
    }

    T &operator()(std::size_t i, std::size_t j) { return data[i * N + j]; }
    const T &operator()(std::size_t i, std::size_t j) const { return data[i * N + j]; }
    T &operator[](std::size_t i) { return data[i]; }
    const T &operator[](std::size_t i) const { return data[i]; }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
    auto cbegin() const { return data.cbegin(); }
    auto cend() const { return data.cend(); }

    void fill(T value) { data.fill(value); }

    T x() const
    {
        static_assert((M >= 1 && N == 1) || (M == 1 && N >= 1));
        return data[0];
    }

    T y() const
    {
        static_assert((M >= 2 && N == 1) || (M == 1 && N >= 2));
        return data[1];
    }

    T z() const
    {
        static_assert((M >= 3 && N == 1) || (M == 1 && N >= 3));
        return data[2];
    }

    T w() const
    {
        static_assert((M >= 4 && N == 1) || (M == 1 && N >= 4));
        return data[3];
    }

    T &x()
    {
        static_assert((M >= 1 && N == 1) || (M == 1 && N >= 1));
        return data[0];
    }

    T &y()
    {
        static_assert((M >= 2 && N == 1) || (M == 1 && N >= 2));
        return data[1];
    }

    T &z()
    {
        static_assert((M >= 3 && N == 1) || (M == 1 && N >= 3));
        return data[2];
    }

    T &w()
    {
        static_assert((M >= 4 && N == 1) || (M == 1 && N >= 4));
        return data[3];
    }

    fixed_mat operator+() const { return *this; }
    fixed_mat operator-() const
    {
        fixed_mat result;
        for (std::size_t i = 0; i < M * N; ++i)
        {
            result.data[i] = -data[i];
        }
        return result;
    }

    template <scalar S>
    fixed_mat &operator+=(const fixed_mat<S, M, N> &m)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] += m.data[i];
        }
        return *this;
    }

    template <scalar S>
    fixed_mat &operator-=(const fixed_mat<S, M, N> &m)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] -= m.data[i];
        }
        return *this;
    }

    template <scalar S>
    fixed_mat &operator*=(S value)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] *= value;
        }
        return *this;
    }

    template <scalar S>
    fixed_mat &operator/=(S value)
    {
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] /= value;
        }
        return *this;
    }

    auto norm() const -> decltype(std::sqrt(data[0] * data[0]))
    {
        T result = 0;
        for (std::size_t i = 0; i < M * N; ++i)
        {
            result += data[i] * data[i];
        }
        return std::sqrt(result);
    }

    auto normalized() const -> fixed_mat<decltype(data[0] / norm()), M, N>
    {
        using S = decltype(data[0] / norm());
        fixed_mat<S, M, N> result;
        auto n = norm();
        for (std::size_t i = 0; i < M * N; ++i)
        {
            result.data[i] = data[i] / n;
        }
        return result;
    }

    void normalize()
    {
        auto n = norm();
        for (std::size_t i = 0; i < M * N; ++i)
        {
            data[i] /= n;
        }
    }

    fixed_mat<T, N, M> transpose() const
    {
        fixed_mat<T, N, M> result;
        for (std::size_t i = 0; i < M; ++i)
        {
            for (std::size_t j = 0; j < N; ++j)
            {
                result.data[j * M + i] = data[i * N + j];
            }
        }
        return result;
    }

    fixed_mat<T, N, M> adjoint() const
    {
        fixed_mat<T, N, M> result;
        for (std::size_t i = 0; i < M; ++i)
        {
            for (std::size_t j = 0; j < N; ++j)
            {
                if constexpr (is_complex_v<T>)
                    result.data[j * M + i] = std::conj(data[i * N + j]);
                else
                    result.data[j * M + i] = data[i * N + j];
            }
        }
        return result;
    }

    auto diag() const -> fixed_mat<T, std::min(M, N), 1>
    {
        fixed_mat<T, std::min(M, N), 1> result;
        for (std::size_t i = 0; i < std::min(M, N); ++i)
        {
            result[i] = data[i * N + i];
        }
        return result;
    }

    static fixed_mat identity()
    {
        static_assert(M == N, "M must be equal to N");
        fixed_mat result;
        result.data.fill(0);
        for (std::size_t i = 0; i < M; ++i)
        {
            result.data[i * N + i] = 1;
        }
        return result;
    }

    static fixed_mat zero()
    {
        fixed_mat result;
        result.data.fill(0);
        return result;
    }

    static fixed_mat diagm(const fixed_mat<T, M, 1> &v)
    {
        static_assert(M == N, "M must be equal to N");
        fixed_mat result;
        result.data.fill(0);
        for (std::size_t i = 0; i < M; ++i)
        {
            result.data[i * N + i] = v[i];
        }
        return result;
    }

    static fixed_mat diagm(const fixed_mat<T, 1, N> &v)
    {
        static_assert(M == N, "M must be equal to N");
        fixed_mat result;
        result.data.fill(0);
        for (std::size_t i = 0; i < N; ++i)
        {
            result.data[i * N + i] = v[i];
        }
        return result;
    }
};

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto operator+(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2) -> fixed_mat<decltype(R() + S()), M, N>
{
    using T = decltype(R() + S());
    fixed_mat<T, M, N> m;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        m[i] = m1[i] + m2[i];
    }
    return m;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto operator-(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2) -> fixed_mat<decltype(R() - S()), M, N>
{
    using T = decltype(R() - S());
    fixed_mat<T, M, N> m;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        m[i] = m1[i] - m2[i];
    }
    return m;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto operator*(const fixed_mat<R, M, N> &m, const S &s) -> fixed_mat<decltype(R() * S()), M, N>
{
    using T = decltype(R() * S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        ret[i] = m[i] * s;
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto operator*(const R &s, const fixed_mat<S, M, N> &m) -> fixed_mat<decltype(R() * S()), M, N>
{
    using T = decltype(R() * S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        ret[i] = s * m[i];
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto operator/(const fixed_mat<R, M, N> &m, const S &s) -> fixed_mat<decltype(R() / S()), M, N>
{
    using T = decltype(R() / S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        ret[i] = m[i] / s;
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t K, std::size_t N>
auto operator*(const fixed_mat<R, M, K> &m1, const fixed_mat<S, K, N> &m2) -> fixed_mat<decltype(R() * S()), M, N>
{
    using T = decltype(R() * S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M; ++i)
    {
        for (std::size_t j = 0; j < N; ++j)
        {
            ret(i, j) = 0;
            for (std::size_t k = 0; k < K; ++k)
            {
                ret(i, j) += m1(i, k) * m2(k, j);
            }
        }
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto ele_mul(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2) -> fixed_mat<decltype(R() * S()), M, N>
{
    using T = decltype(R() * S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        ret[i] = m1[i] * m2[i];
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
auto ele_div(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2) -> fixed_mat<decltype(R() / S()), M, N>
{
    using T = decltype(R() / S());
    fixed_mat<T, M, N> ret;
    for (std::size_t i = 0; i < M * N; ++i)
    {
        ret[i] = m1[i] / m2[i];
    }
    return ret;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
bool operator==(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2)
{
    for (std::size_t i = 0; i < M * N; ++i)
    {
        if (m1[i] != m2[i])
        {
            return false;
        }
    }
    return true;
}

template <scalar R, scalar S, std::size_t M, std::size_t N>
bool operator!=(const fixed_mat<R, M, N> &m1, const fixed_mat<S, M, N> &m2)
{
    return !(m1 == m2);
}

template <scalar T, std::size_t N>
using fixed_vec = fixed_mat<T, N, 1>;

template <scalar T, std::size_t N>
using fixed_row_vec = fixed_mat<T, 1, N>;

template <scalar T>
using vec2 = fixed_vec<T, 2>;

template <scalar T>
using vec3 = fixed_vec<T, 3>;

template <scalar T>
using vec4 = fixed_vec<T, 4>;

template <scalar T>
using row_vec2 = fixed_row_vec<T, 2>;

template <scalar T>
using row_vec3 = fixed_row_vec<T, 3>;

template <scalar T>
using row_vec4 = fixed_row_vec<T, 4>;

template <scalar T>
using mat2x2 = fixed_mat<T, 2, 2>;

template <scalar T>
using mat3x3 = fixed_mat<T, 3, 3>;

template <scalar T>
using mat4x4 = fixed_mat<T, 4, 4>;

using vec2f = vec2<float>;
using vec3f = vec3<float>;
using vec4f = vec4<float>;
using vec2d = vec2<double>;
using vec3d = vec3<double>;
using vec4d = vec4<double>;

using row_vec2f = row_vec2<float>;
using row_vec3f = row_vec3<float>;
using row_vec4f = row_vec4<float>;
using row_vec2d = row_vec2<double>;
using row_vec3d = row_vec3<double>;
using row_vec4d = row_vec4<double>;

using mat2x2f = mat2x2<float>;
using mat3x3f = mat3x3<float>;
using mat4x4f = mat4x4<float>;
using mat2x2d = mat2x2<double>;
using mat3x3d = mat3x3<double>;
using mat4x4d = mat4x4<double>;

using rgb = vec3<uint8_t>;
using rgba = vec4<uint8_t>;

template <scalar T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const fixed_vec<T, N> &v)
{
    os << "(";
    for (std::size_t i = 0; i < N; ++i)
    {
        os << v[i];
        if (i < N - 1)
        {
            os << ", ";
        }
    }
    os << ")'";
    return os;
}

template <scalar T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const fixed_row_vec<T, N> &v)
{
    os << "(";
    for (std::size_t i = 0; i < N; ++i)
    {
        os << v[i];
        if (i < N - 1)
        {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

template <scalar T, std::size_t M, std::size_t N>
std::ostream &operator<<(std::ostream &os, const fixed_mat<T, M, N> &m)
{
    os << "[";
    for (std::size_t i = 0; i < M; ++i)
    {
        os << m(i, 0);
        for (std::size_t j = 1; j < N; ++j)
        {
            os << ", " << m(i, j);
        }
        if (i < M - 1)
        {
            os << std::endl;
        }
    }
    os << "]";
    return os;
}

template <scalar R, scalar S, std::size_t N>
auto dot(const fixed_vec<R, N> &u, const fixed_vec<S, N> &v) -> decltype(u[0] * v[0])
{
    using T = decltype(u[0] * v[0]);
    T s = 0;
    for (std::size_t i = 0; i < N; ++i)
    {
        s += u[i] * v[i];
    }
    return s;
}

template <scalar R, scalar S>
auto cross(const vec3<R> &u, const vec3<S> &v) -> vec3<decltype(u[0] * v[0])>
{
    using T = decltype(u[0] * v[0]);
    vec3<T> w;
    w[0] = u[1] * v[2] - u[2] * v[1];
    w[1] = u[2] * v[0] - u[0] * v[2];
    w[2] = u[0] * v[1] - u[1] * v[0];
    return w;
}

template <scalar R, scalar S>
auto cos(const vec3<R> &u, const vec3<S> &v) -> decltype(dot(u, v) / (u.norm() * v.norm()))
{
    return dot(u, v) / (u.norm() * v.norm());
}

} // end namespace games

#endif // GAMES_MAT_HPP