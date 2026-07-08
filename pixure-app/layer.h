/**
 * Date: 3/30/2026
 * Authors: Luciano Olivares
 *
 * This is a header for the layer class, defining it's methods and variables.
 */

#ifndef LAYER_H
#define LAYER_H

#include "point2d.h"
#include <QColor>
#include <unordered_map>
#include <unordered_set>
#include <json.hpp>
using json = nlohmann::json;

using std::unordered_set;
using std::unordered_map;

class Layer
{

    // Allows access to JSON for (de)serialization
    friend void to_json(json& j, const Layer& layer);
    friend void from_json(const json& j, Layer& layer);

public:

    /**
     * @brief Returns a constant reference to the pixel map.
     * @return const std::unordered_map<Point2D, QColor, PointHasher>&
     */
    const std::unordered_map<Point2D, QColor> getPixels() const;

    /**
     * @brief Layer - Holds pixel data and manipulation methods for a single layer
     * @param width - The width of the layer
     * @param height - The height of the layer
     */
    explicit Layer(int width, int height);

    /**
     * @brief Layer - Copy constructor for the Layer class
     * @param other - The other layer to copy
     */
    Layer(const Layer& other);

    /**
     * @brief operator= - Assignment operator for the Layer class
     * @param other - The other layer to assign
     */
    Layer& operator=(const Layer& other);

    /**
     * @brief ~Layer - Destructor for the Layer class
     */
    ~Layer();

    /**
     * @brief drawPixel - Draws a pixel to the canvas
     * @param pixelPosition - The position of the pixel
     * @param color - The color of the pixel
     */
    void drawPixel(Point2D pixel, QColor color);

    /**
     * @brief erasePixel - Removes a drawn pixel
     * @param pixelPosition - The position of the pixel to erase
     */
    void erasePixel(Point2D pixel);

    /**
     * @brief fillPixel - Fills an area with a color, returns the affected area
     * @param pixel - The seed pixel to fill from
     * @param color - The color to fill with
     */
    unordered_set<Point2D> fillPixel(Point2D pixel, QColor color);

    /**
     * @brief shapeSelect - Selects a shape at the given pixel's position
     * @param pixelPosition - Reference to the pixel
     */
    unordered_set<Point2D> shapeSelect(Point2D pixel);

    /**
     * @brief colorShift - Shifts the color of an area closer to the desired shift color, changing the hue of effected pixels.
     * @param shift - The color shift to apply
     * @param area - Reference to the set of coordinates to shift
     */
    unordered_map<Point2D, QColor> colorShift(Point2D pixel, QColor shiftColor);

    /**
     * @brief resizeLayer - Updates boundaries and removes out-of-bounds pixels
     * @param newWidth - New width of the layer
     * @param newHeight - New height of the layer
     */
    void resizeLayer(int newWidth, int newHeight);

private:
    unordered_map<Point2D, QColor> pixels;
    int layerWidth;
    int layerHeight;

    /**
     * @brief scan - Recursive scanline method branching from a seed pixel
     * @param seedPixel - The pixel to branch from when scanning
     */
    void scan(Point2D seedPixel, unordered_set<Point2D>& selection);
};

#endif // LAYER_H
