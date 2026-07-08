/**
 * Date: 3/26/2026
 * Authors: Luciano Olivares
 *
 * This layer class defines components of a frame, with each layer being individual sprite drawings, layering up to a frame.
 * Overlapping pixels' priority on the frame are determined by layer order.
 */

#include "layer.h"

Layer::Layer(int width, int height)
    : layerWidth(width)
    , layerHeight(height)
{
}

Layer::Layer(const Layer& other)
    : pixels(other.pixels)
    , layerWidth(other.layerWidth)
    , layerHeight(other.layerHeight)
{
}

Layer& Layer::operator=(const Layer& other)
{
    if (this == &other) return *this;
    pixels = other.pixels;
    layerWidth = other.layerWidth;
    layerHeight = other.layerHeight;

    return *this;
}

Layer::~Layer()
{
    // pixels unordered_map cleans itself up automatically
}

void Layer::drawPixel(Point2D  pixel, QColor color)
{
    pixels[pixel] = color;
}

void Layer::erasePixel(Point2D  pixel)
{
    pixels.erase(pixel);
}

unordered_set<Point2D> Layer::fillPixel(Point2D pixel, QColor color)
{
    unordered_set<Point2D> area = shapeSelect(pixel);
    for(const Point2D& pixel : area)
    {
        pixels.at(pixel) = color;
    }
    return area;
}

unordered_set<Point2D> Layer::shapeSelect(Point2D pixel)
{
    unordered_set<Point2D> selection;
    scan( pixel, selection);
    return selection;
}

unordered_map<Point2D, QColor> Layer::colorShift(Point2D pixel, QColor shiftColor)
{
    unordered_set<Point2D> area = shapeSelect(pixel);
    unordered_map<Point2D, QColor> effectedPixels;
    for(const Point2D& pixel : area)
    {
        QColor mergedColor(
            (shiftColor.red()   + pixels.at(pixel).red())   / 2,
            (shiftColor.green() + pixels.at(pixel).green()) / 2,
            (shiftColor.blue()  + pixels.at(pixel).blue())  / 2);
        pixels.at(pixel) = mergedColor;
        effectedPixels[pixel] = mergedColor;
    }
    return effectedPixels;
}

void Layer::scan(Point2D seedPixel, unordered_set<Point2D>& selection)
{
    if(!pixels.count(seedPixel) || selection.count(seedPixel))
        return;

    QColor targetColor = pixels.at(seedPixel);
    Point2D travelingPixel = seedPixel;

    // scan to left border
    while(pixels.count(travelingPixel) && pixels.at(travelingPixel) == targetColor)
    {
        selection.insert(travelingPixel);

        Point2D above(travelingPixel.x, travelingPixel.y - 1);
        Point2D below(travelingPixel.x, travelingPixel.y + 1);

        // checks vertically adjacent pixels
        if(pixels.count(above) && pixels.at(above) == targetColor)
            scan(above, selection);
        if(pixels.count(below) && pixels.at(below) == targetColor)
            scan(below, selection);

        travelingPixel = Point2D(travelingPixel.x - 1, travelingPixel.y);
    }

    // reset and scan to right border
    travelingPixel = Point2D(seedPixel.x + 1, seedPixel.y);
    while(pixels.count(travelingPixel) && pixels.at(travelingPixel) == targetColor)
    {
        selection.insert(travelingPixel);

        Point2D above(travelingPixel.x, travelingPixel.y - 1);
        Point2D below(travelingPixel.x, travelingPixel.y + 1);

        // checks vertically adjacent pixels
        if(pixels.count(above) && pixels.at(above) == targetColor)
            scan(above, selection);
        if(pixels.count(below) && pixels.at(below) == targetColor)
            scan(below, selection);

        travelingPixel = Point2D(travelingPixel.x + 1, travelingPixel.y);
    }
}

/**
 * @brief Implementation of getPixels.
 * Returns the internal map by reference to avoid a heavy copy.
 */
const std::unordered_map<Point2D, QColor> Layer::getPixels() const
{
    return pixels;
}

void Layer::resizeLayer(int newWidth, int newHeight)
{
    // Update the internal dimensions
    layerWidth = newWidth;
    layerHeight = newHeight;

    for (auto it = pixels.begin(); it != pixels.end(); ) {
        Point2D pos = it->first;

        if (pos.x >= newWidth || pos.y >= newHeight) {
            it = pixels.erase(it); // Remove and move to next
        } else {
            ++it; // Keep and move to next
        }
    }
}
