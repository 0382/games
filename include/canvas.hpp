#pragma once
#ifndef GAMES_CANVAS_HPP
#define GAMES_CANVAS_HPP

#include "color.hpp"
#include "geometry2d.hpp"
#include "window.hpp"
#include <numbers>

namespace games
{

class Canvas
{
  private:
    RawCanvas m_canvas;

    static constexpr int SN = 4;
    uint8_t *m_subpixels;

    uint8_t *sp_at(int x, int y) { return m_subpixels + 3 * (y * width() * SN + x); }
    int sp_width() const { return SN * width(); }
    int sp_height() const { return SN * height(); }
    void set_subpixel(int x, int y, rgb color)
    {
        uint8_t *p = sp_at(x, y);
        p[0] = color.r;
        p[1] = color.g;
        p[2] = color.b;
    }
    void mean_subpixel()
    {
        for (int y = 0; y < height(); ++y)
        {
            for (int x = 0; x < width(); ++x)
            {
                unsigned r = 0, g = 0, b = 0;
                for (int sy = 0; sy < SN; ++sy)
                {
                    int gy = y * SN + sy;
                    for (int sx = 0; sx < SN; ++sx)
                    {
                        int gx = x * SN + sx;
                        uint8_t *p = sp_at(gx, gy);
                        r += p[0];
                        g += p[1];
                        b += p[2];
                    }
                }
                r = (r + SN * SN / 2) / (SN * SN);
                g = (g + SN * SN / 2) / (SN * SN);
                b = (b + SN * SN / 2) / (SN * SN);
                m_canvas.set_pixel(x, y, static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
            }
        }
    }

    void sp_fill_rect(int x0, int y0, int x1, int y1, rgb color)
    {
        for (int y = y0; y < y1; ++y)
        {
            for (int x = x0; x < x1; ++x)
            {
                set_subpixel(x, y, color);
            }
        }
    }

  public:
    Canvas(int width, int height) : m_canvas(width, height) { m_subpixels = new uint8_t[3 * width * height * SN * SN]; }
    ~Canvas() { delete[] m_subpixels; }

    RawCanvas &get_raw_canvas() { return m_canvas; }
    int width() const { return m_canvas.width(); }
    int height() const { return m_canvas.height(); }
    void beginpaint() { m_canvas.beginpaint(); }
    void endpaint()
    {
        mean_subpixel();
        m_canvas.endpaint();
    }
    void fill(rgb color) { sp_fill_rect(0, 0, sp_width(), sp_height(), color); }
    void save_bmp(const char *filename)
    {
        mean_subpixel();
        m_canvas.save_bmp(filename);
    }

    void fill(const geo2d::circle &c, rgb color)
    {
        int x0 = std::round((c.center[0] - c.radius) * SN);
        int x1 = std::round((c.center[0] + c.radius) * SN);
        int y0 = std::round((c.center[1] - c.radius) * SN);
        int y1 = std::round((c.center[1] + c.radius) * SN);
        for (int y = y0; y < y1; ++y)
        {
            if (unsigned(y) >= sp_height())
                continue;
            for (int x = x0; x < x1; ++x)
            {
                if (unsigned(x) >= sp_width())
                    continue;
                float fx = (x + 0.5) / SN;
                float fy = (y + 0.5) / SN;
                if ((c.center - vec2f(fx, fy)).norm() <= c.radius)
                    set_subpixel(x, y, color);
            }
        }
    }

    void draw(const geo2d::circle &c, float line_width, rgb color)
    {
        float R1 = c.radius - line_width / 2;
        float R2 = c.radius + line_width / 2;
        int x0 = std::round((c.center[0] - R2) * SN);
        int x1 = std::round((c.center[0] + R2) * SN);
        int y0 = std::round((c.center[1] - R2) * SN);
        int y1 = std::round((c.center[1] + R2) * SN);
        for (int y = y0; y < y1; ++y)
        {
            if (unsigned(y) >= sp_height())
                continue;
            for (int x = x0; x < x1; ++x)
            {
                if (unsigned(x) >= sp_width())
                    continue;
                float fx = (x + 0.5) / SN;
                float fy = (y + 0.5) / SN;
                float d = (c.center - vec2f(fx, fy)).norm();
                if (d >= R1 && d <= R2)
                    set_subpixel(x, y, color);
            }
        }
    }

    void fill(const geo2d::triangle &t, rgb color)
    {
        int xmin = std::max(0, static_cast<int>(std::round(t.xmin() * SN)));
        int xmax = std::min(sp_width(), static_cast<int>(std::round(t.xmax() * SN)));
    }

    void fill(const geo2d::rect &r, rgb color)
    {
        static constexpr float pi = std::numbers::pi_v<float>;
        if (r.angle == 0 || r.angle == 2 * pi || r.angle == -2 * pi)
        {
            auto [x0, x1] = r.xminmax();
            auto [y0, y1] = r.yminmax();
            int ix0 = std::max(0, static_cast<int>(std::round(x0 * SN)));
            int ix1 = std::min(sp_width(), static_cast<int>(std::round(x1 * SN)));
            int iy0 = std::max(0, static_cast<int>(std::round(y0 * SN)));
            int iy1 = std::min(sp_height(), static_cast<int>(std::round(y1 * SN)));
            sp_fill_rect(ix0, iy0, ix1, iy1, color);
            return;
        }
        float c = std::cos(r.angle);
        float s = std::sin(r.angle);
        float hw = r.width / 2;
        float hh = r.height / 2;
        float x0 = r.center[0] - hw * c - hh * s;
        float x1 = r.center[0] + hw * c - hh * s;
        float x2 = r.center[0] + hw * c + hh * s;
        float x3 = r.center[0] - hw * c + hh * s;
        float y0 = r.center[1] - hw * s + hh * c;
        float y1 = r.center[1] + hw * s + hh * c;
        float y2 = r.center[1] + hw * s - hh * c;
        float y3 = r.center[1] - hw * s - hh * c;
        if (std::abs(r.angle) < pi / 4)
        {
            float Dy = r.height * SN / c;                        // 竖直方向的像素数
            int xmin = std::round((r.angle > 0 ? x0 : x3) * SN); // 左边的顶点
            int xmax = std::round((r.angle > 0 ? x2 : x1) * SN); // 右边的顶点
            int xc1 = std::round((r.angle > 0 ? x3 : x0) * SN);  // 中间的顶点
            int xc2 = std::round((r.angle > 0 ? x1 : x2) * SN);
            float t = (xmin - x3 * SN) / (x2 * SN - x3 * SN);
            float fy = lerp(y3, y2, t) * SN;
            int ymin = std::round(fy);
            float e = ymin + 0.5 - fy;
            float slop = std::abs((y2 - y3) / (x2 - x3));
            float incy = y2 > y3 ? 1 : -1;
            for (int x = xmin; x < xmax; ++x)
            {
                if (unsigned(x) >= sp_width())
                    continue;
                for (int y = ymin; y < ymin + Dy - e; ++y)
                {
                    if (unsigned(y) >= sp_height())
                        continue;
                    if (x >= xc1 && x < xc2)
                        set_subpixel(x, y, color);
                    else
                    {
                        float t1 = (y - y3 * SN) / (y0 * SN - y3 * SN);
                        float t2 = (y - y2 * SN) / (y1 * SN - y2 * SN);
                        float tx1 = lerp(x3, x0, t1) * SN;
                        float tx2 = lerp(x2, x1, t2) * SN;
                        if ((x - tx1) * (x - tx2) <= 0)
                            set_subpixel(x, y, color);
                    }
                }
                e += slop;
                if (e >= 1)
                {
                    e -= 1;
                    ymin += incy;
                }
            }
        }
    }

    void draw(const geo2d::line &l, float line_width, rgb color)
    {
        float height = line_width;
        float width = (l.stop - l.start).norm() + height;
        vec2f center = (l.start + l.stop) / 2;
        float angle = std::atan2(l.stop[1] - l.start[1], l.stop[0] - l.start[0]);
        geo2d::rect r{height, width, center, angle};
        fill(r, color);
    }
};

} // end namespace games

#endif // GAMES_CANVAS_HPP