/**
 * Date: 3/26/26
 * Authors: Sebastian Flores, Joshua Green
 *
 * This class serves as the primary key for pixel storage in Layers.
 * It includes equality comparison to support use in associative containers.
 */

#ifndef POINT2D_H
#define POINT2D_H
#include <functional>

class Point2D
{
    public:
        int x, y;

        Point2D(int x = 0, int y = 0);

        Point2D(const Point2D& other);

        bool operator==(const Point2D& other) const;
    };

    namespace std {
        template <>
        struct hash<Point2D> {
            std::size_t operator()(const Point2D& p) const noexcept {
                return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
            }
    };
}

#endif // POINT2D_H
