#pragma once
#ifndef GAMES_COLOR_HPP
#define GAMES_COLOR_HPP

#include <algorithm>
#include <cstdint>

namespace games
{

struct rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    rgb &operator*=(double k)
    {
        r *= k;
        g *= k;
        b *= k;
        return *this;
    }
    rgb &operator/=(double k) { return *this *= 1.0 / k; }

    static rgb light_white() { return {255, 255, 255}; } // 亮白
    static rgb white() { return {200, 200, 200}; }       // 白
    static rgb light_gray() { return {150, 150, 150}; }  // 亮灰
    static rgb gray() { return {100, 100, 100}; }        // 灰
    static rgb dark_gray() { return {50, 50, 50}; }      // 暗灰
    static rgb black() { return {0, 0, 0}; }             // 黑
    static rgb red() { return {255, 0, 0}; }             // 红
    static rgb green() { return {0, 255, 0}; }           // 绿
    static rgb blue() { return {0, 0, 255}; }            // 蓝
    static rgb yellow() { return {255, 255, 0}; }        // 黄
    static rgb cyan() { return {0, 255, 255}; }          // 青
    static rgb magenta() { return {255, 0, 255}; }       // 品红
    static rgb orange() { return {255, 165, 0}; }        // 橙
    static rgb purple() { return {128, 0, 128}; }        //    紫
    static rgb pink() { return {255, 192, 203}; }        // 粉
    static rgb brown() { return {165, 42, 42}; }         // 棕
    static rgb gold() { return {255, 215, 0}; }          // 金
    static rgb silver() { return {192, 192, 192}; }      // 银
    static rgb beige() { return {245, 245, 220}; }       // 米
    static rgb maroon() { return {128, 0, 0}; }          // 栗
    static rgb mint() { return {189, 252, 201}; }        // 薄荷
    static rgb teal() { return {0, 128, 128}; }          // 青绿
    static rgb lavender() { return {230, 230, 250}; }    // 薰衣草
    static rgb olive() { return {128, 128, 0}; }         // 橄榄
    static rgb coral() { return {255, 127, 80}; }        // 珊瑚
    static rgb salmon() { return {250, 128, 114}; }      // 鲑鱼
    static rgb khaki() { return {240, 230, 140}; }       // 卡其
    static rgb indigo() { return {75, 0, 130}; }         // 靛
    static rgb turquoise() { return {64, 224, 208}; }    // 绿松石
    static rgb violet() { return {238, 130, 238}; }      // 紫罗兰
    static rgb sky_blue() { return {135, 206, 235}; }    // 天蓝
    static rgb azure() { return {240, 255, 255}; }       // 蔚蓝
};

inline bool operator==(const rgb &c1, const rgb &c2) { return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b; }
inline bool operator!=(const rgb &c1, const rgb &c2) { return !(c1 == c2); }

inline rgb operator*(const rgb &c, double k)
{
    return {static_cast<uint8_t>(c.r * k), static_cast<uint8_t>(c.g * k), static_cast<uint8_t>(c.b * k)};
}
inline rgb operator*(double k, const rgb &c) { return c * k; }
inline rgb operator/(const rgb &c, double k) { return c * (1.0 / k); }
inline rgb operator+(const rgb &c1, const rgb &c2)
{
    return {static_cast<uint8_t>(c1.r + c2.r), static_cast<uint8_t>(c1.g + c2.g), static_cast<uint8_t>(c1.b + c2.b)};
}
inline rgb operator-(const rgb &c1, const rgb &c2)
{
    return {static_cast<uint8_t>(c1.r - c2.r), static_cast<uint8_t>(c1.g - c2.g), static_cast<uint8_t>(c1.b - c2.b)};
}

inline rgb mix(double k, const rgb &c1, const rgb &c2)
{
    k = std::clamp(k, 0.0, 1.0);
    return {static_cast<uint8_t>(c1.r * k + c2.r * (1 - k)), //
            static_cast<uint8_t>(c1.g * k + c2.g * (1 - k)), //
            static_cast<uint8_t>(c1.b * k + c2.b * (1 - k))};
}

} // end namespace games

#endif // GAMES_COLOR_HPP