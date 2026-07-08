/**
 * Date:
 * Authors:
 */

#include "point2d.h"

/**
 * @brief Implementation of the Point2D constructor.
 * Uses a member initializer list to assign x and y coordinates.
 * This is more efficient than assignment inside the constructor body.
 * @param x The horizontal coordinate value.
 * @param y The vertical coordinate value.
 */
Point2D::Point2D(int x, int y) : x(x), y(y){}

/**
 * @brief Implementation of the Copy Constructor.
 * Copies x and y from the 'other' point into this new point.
 */
Point2D::Point2D(const Point2D& other) : x(other.x), y(other.y) {}

/**
 * @brief Equality operator implementation.
 */
bool Point2D::operator==(const Point2D& other) const {
    return (x == other.x && y == other.y);
}
