#include "g2d.hpp"

using namespace games;

int main()
{
    vec3f a = {1.0f, 2.0f, 3.0f};
    vec3f b = {4.0f, 5.0f};
    std::cout << "a.x() = " << a.x() << std::endl;
    std::cout << "a.y() = " << a.y() << std::endl;
    std::cout << "a.z() = " << a.z() << std::endl;
    // std::cout << "a.w() = " << a.w() << std::endl; // static_assert(N >= 4) will fail
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "+a = " << +a << std::endl;
    std::cout << "-a = " << -a << std::endl;
    std::cout << "a + b = " << a + b << std::endl;
    std::cout << "a - b = " << a - b << std::endl;
    std::cout << "3.0 * a = " << 3 * a << std::endl;
    std::cout << "a * 2.0 = " << a * 2 << std::endl;
    std::cout << "a / 2.0 = " << a / 2 << std::endl;
    std::cout << "cross(a, b) = " << cross(a, b) << std::endl;
    std::cout << "dot(a, b) = " << dot(a, b) << std::endl;
    std::cout << "cos(a, b) = " << cos(a, b) << std::endl;
    std::cout << "a.norm() = " << a.norm() << std::endl;
    std::cout << "a.normalized() = " << a.normalized() << std::endl;
    a.normalize();
    std::cout << "unit vector of a = " << a << std::endl;
    std::cout << "a.adjoint() = " << a.transpose() << std::endl;

    vec2d c = {1.0, 2.0};
    mat2x2d r = rotation2d(deg2rad(45.0));
    c *= std::sqrt(2.0);
    std::cout << "c = " << c << std::endl;
    std::cout << "r = \n" << r << std::endl;
    std::cout << "r * c = " << r * c << std::endl;
    std::cout << "rotate(c, 45o) = " << rotate(c, deg2rad(45)) << std::endl;
    std::cout << "rotate(c, 45o) = " << transform2d<double>::rotation(deg2rad(45)) * c << std::endl;
    return 0;
}