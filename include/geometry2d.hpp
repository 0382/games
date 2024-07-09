#pragma once
#ifndef GAMES_GEOMETRY2D_HPP
#define GAMES_GEOMETRY2D_HPP

#include "mat.hpp"

namespace games
{

namespace geo2d
{

struct line
{
    vec2f start;
    vec2f stop;

    line(vec2f s, vec2f e, float w = 1) : start(s), stop(e) {}
    line(float x0, float y0, float x1, float y1, float w = 1) : start(x0, y0), stop(x1, y1) {}
    float slope() const { return (stop[1] - start[1]) / (stop[0] - start[0]); }
};

struct circle
{
    vec2f center;
    float radius;

    circle(vec2f c, float r) : center(c), radius(r) {}
    circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct ellipse
{
    vec2f center;
    float a;
    float b;
    float angle;

    ellipse(vec2f c, float ra, float rb, float angle = 0) : center(c), a(ra), b(rb), angle(normalize_rad_Cn(angle, 2))
    {}
    ellipse(float x, float y, float ra, float rb, float angle = 0)
        : center(x, y), a(ra), b(rb), angle(normalize_rad_Cn(angle, 2))
    {}
};

struct triangle
{
    vec2f a;
    vec2f b;
    vec2f c;

    triangle(vec2f a, vec2f b, vec2f c) : a(a), b(b), c(c) {}
    triangle(float x0, float y0, float x1, float y1, float x2, float y2) : a(x0, y0), b(x1, y1), c(x2, y2) {}
    float xmax() const { return std::max({a[0], b[0], c[0]}); }
    float xmin() const { return std::min({a[0], b[0], c[0]}); }
    float ymax() const { return std::max({a[1], b[1], c[1]}); }
    float ymin() const { return std::min({a[1], b[1], c[1]}); }
};

struct rect
{
    float height;
    float width;
    vec2f center;
    float angle;

    rect(float h, float w, vec2f c, float angle = 0) : height(h), width(w), center(c), angle(normalize_rad_Cn(angle, 2))
    {}
    rect(float h, float w, float x, float y, float angle = 0)
        : height(h), width(w), center(x, y), angle(normalize_rad_Cn(angle, 2))
    {}
    std::pair<float, float> xminmax() const
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        float hw = width / 2;
        float hh = height / 2;
        float x0 = center[0] + hw * c - hh * s;
        float x1 = center[0] - hw * c + hh * s;
        return std::make_pair(std::min(x0, x1), std::max(x0, x1));
    }
    std::pair<float, float> yminmax() const
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        float hw = width / 2;
        float hh = height / 2;
        float y0 = center[1] + hw * s + hh * c;
        float y1 = center[1] - hw * s - hh * c;
        return std::make_pair(std::min(y0, y1), std::max(y0, y1));
    }
};

}; // namespace geo2d

} // end namespace games

#endif // GAMES_GEOMETRY2D_HPP