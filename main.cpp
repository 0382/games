#include "canvas.hpp"
#include <deque>
#include <iostream>

using namespace games;

struct DoublePendulum
{
    static constexpr double g = 9.8;
    static constexpr double dt = 0.001;
    rgb color;
    double l1;
    double l2;
    double m1;
    double m2;
    double theta1 = 0;
    double theta2 = 0;
    double omega1 = 0;
    double omega2 = 0;
    std::deque<std::pair<double, double>> trace;
    DoublePendulum(double l1, double l2, double m1, double m2, rgb color) : l1(l1), l2(l2), m1(m1), m2(m2), color(color)
    {}
    void start(double theta1_, double theta2_, double omega1_, double omega2_)
    {
        theta1 = theta1_;
        theta2 = theta2_;
        omega1 = omega1_;
        omega2 = omega2_;
    }
    std::pair<double, double> solve(double a, double b, double c, double d, double v1, double v2)
    {
        double det = a * d - b * c;
        double num1 = d * v1 - b * v2;
        double num2 = -c * v1 + a * v2;
        return {num1 / det, num2 / det};
    }

    void step()
    {
        for (int i = 0; i < 100; ++i)
        {
            double a = (m1 + m2) * l1;
            double b = m2 * l2 * std::cos(theta1 - theta2);
            double c = m2 * l1 * std::cos(theta1 - theta2);
            double d = m2 * l2;
            double v1 = -m2 * l2 * omega2 * omega2 * std::sin(theta1 - theta2) - (m1 + m2) * g * std::sin(theta1);
            double v2 = m2 * l1 * omega1 * omega1 * std::sin(theta1 - theta2) - m2 * g * std::sin(theta2);
            auto [d1, d2] = solve(a, b, c, d, v1, v2);
            omega1 += d1 * dt;
            omega2 += d2 * dt;
            theta1 += omega1 * dt;
            theta2 += omega2 * dt;
        }
        trace.push_back({theta1, theta2});
        if (trace.size() > 50)
        {
            trace.pop_front();
        }
    }
    void draw(Canvas &canvas, int x, int y, double scale)
    {
        double R = 10;
        float x0 = x;
        float y0 = y;
        float x1 = x0 + l1 * scale * std::sin(theta1);
        float y1 = y0 + l1 * scale * std::cos(theta1);
        float x2 = x1 + l2 * scale * std::sin(theta2);
        float y2 = y1 + l2 * scale * std::cos(theta2);
        canvas.draw(geo2d::line(x0, y0, x1, y1), 1, rgb{255, 255, 0});
        canvas.draw(geo2d::line(x1, y1, x2, y2), 1, rgb{255, 255, 0});
        canvas.fill(geo2d::circle(x1, y1, R * scale), color);
        canvas.fill(geo2d::circle(x2, y2, R * scale), color);
        for (auto &p : trace)
        {
            int x1 = x0 + l1 * scale * std::sin(p.first);
            int y1 = y0 + l1 * scale * std::cos(p.first);
            int x2 = x1 + l2 * scale * std::sin(p.second);
            int y2 = y1 + l2 * scale * std::cos(p.second);
            rgb c = color;
            c.r = c.r * 0.7;
            c.g = c.g * 0.7;
            c.b = c.b * 0.7;
            canvas.fill(geo2d::circle(x2, y2, R * scale * 0.3), c);
        }
    }
};

int main()
{
    Application app;
    const int width = 800;
    const int height = 600;
    Canvas canvas(width, height);
    auto size_policy = SizePolicy::ratio(double(width) / height);
    MainWindow win(size_policy, canvas.get_raw_canvas(), 60);
    if (!win.init(L"Hello world!", width, height))
    {
        return 0;
    }

    std::thread t1(
        [&canvas]
        {
            static constexpr double pi = 3.14159265358979323846;
            DoublePendulum p1(150, 100, 1, 1, rgb{255, 0, 0});
            DoublePendulum p2(150, 100, 1, 1, rgb{0, 255, 0});
            DoublePendulum p3(150, 100, 1, 1, rgb{0, 0, 255});
            p1.start(pi / 2, pi / 2, 0, 0);
            p2.start(pi / 2, pi / 2, 0, 0.01);
            p3.start(pi / 2, pi / 2, 0, -0.01);
            static int count = 0;
            while (true)
            {
                canvas.beginpaint();
                canvas.fill(rgb{128, 128, 128});
                p1.step();
                p2.step();
                p3.step();
                p1.draw(canvas, width / 2, height / 4, 1.5);
                p2.draw(canvas, width / 2, height / 4, 1.5);
                p3.draw(canvas, width / 2, height / 4, 1.5);
                canvas.endpaint();
                if (++count == 1000)
                {
                    canvas.save_bmp("doublependulum.bmp");
                }
            }
        });

    win.show();
    app.exec();
    return 0;
}