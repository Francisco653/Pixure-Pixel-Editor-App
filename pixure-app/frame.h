/**
 * Date: 3/30/2026
 * Authors: Luciano Olivares,
 *
 * This class defines methods and variables for the frame class
 */

#ifndef FRAME_H
#define FRAME_H

#include "layer.h"
#include <unordered_set>
#include <QObject>
#include <QModelIndex>
#include <vector>
#include <json.hpp>

using json = nlohmann::json;
using std::vector;
using std::unordered_set;

class Frame : public QObject
{
    Q_OBJECT

    // Allows JSON (de)serialization.
    friend void to_json(json& j, const Frame& frame);
    friend void from_json(const json& j, Frame& frame);

public:
    /**
     * @brief Frame - A frame of the sprite animation, composed by this frames' layers
     * @param width - The width of the frame
     * @param height - The height of the frame
     */
    Frame(int width, int height, QObject* parent = nullptr);

    /**
     * @brief Frame - Copy constructor for the frame class
     * @param other - The other frame
     */
    Frame(const Frame& other);

    /**
     * @brief ~Frame - Destructor for the frame class
     */
    ~Frame();

    /**
     * @brief operator = - assignment operator
     */
    Frame& operator=(const Frame& other);

    /**
     * @brief composeFrame - combines all layers into a single layer
     */
    Layer composeFrame();

    /**
     * @brief canDeleteLayer - whether the user can delete layers
     * @return bool - true only if there is more than 1 layer
     */
    bool canDeleteLayer();

    /**
     * @brief getPixels - returns composed pixels (all layers)
     */
    std::unordered_map<Point2D, QColor> getPixels();

    /**
     * @brief getActiveLayerPixels - returns only active layer pixels
     */
    std::unordered_map<Point2D, QColor> getActiveLayerPixels();

    /**
     * @brief getLayer - returns the layer at index
     */
    Layer* getLayer(int index);

    /**
     * @brief getActiveLayerIndex - returns index of active layer
     */
    int getActiveLayerIndex();

    /**
     * @brief layerCount - number of layers
     */
    int layerCount();

    /**
     * @brief selectLayer - sets active layer
     */
    void selectLayer(const QModelIndex& index);

    /**
     * @brief addLayer - adds a new layer
     */
    void addLayer();

    /**
     * @brief deleteLayer - deletes active layer
     */
    void deleteLayer();

    /**
     * @brief reorderLayer - reorder layers
     */
    void reorderLayer(int destination, int origin);

    int getWidth();

    int getHeight();

    int activeLayerIndex;
    Layer* activeLayer;
    unordered_set<Point2D> currentSelection;

    /**
     * @brief resizeFrame - resizing the frames whne the canvs size has changed
     * @param newWidth - updated frame width
     * @param newHeight - updated frame height
     */
    void resizeFrame(int newWidth, int newHeight);

private:
    Layer* onionSkinLayer;
    vector<Layer*> layers;
    int frameWidth;
    int frameHeight;

};

#endif // FRAME_H
