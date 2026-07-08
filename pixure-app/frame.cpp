/**
 * Date: 3/30/2026
 * Authors: Luciano Olivares
 *
 * This class defines frames, a component of the canvas, which are comprised of multiple layers. Multiple frames comprise an animation of the sprite.
 */
#include "frame.h"

Frame::Frame(int width, int height, QObject *parent)
    : QObject(parent)
    , layers{}
    , activeLayer(nullptr)
    , onionSkinLayer(nullptr)
    , frameWidth(width)
    , frameHeight(height)

{
    layers.push_back(new Layer(frameWidth, frameHeight));
    activeLayerIndex = 0;
    activeLayer = layers[0];
}

Frame::Frame(const Frame& other)
    : activeLayer(nullptr)
    , onionSkinLayer(nullptr)
    , frameWidth(other.frameWidth)
    , frameHeight(other.frameHeight)
{
    for (Layer* layer : other.layers)
    {
        layers.push_back(new Layer(*layer));
    }

    activeLayerIndex = other.activeLayerIndex;
    activeLayer = layers[activeLayerIndex];
}

Frame::~Frame()
{
    for (Layer* layer : layers)
        delete layer;
    delete onionSkinLayer;
}

int Frame::getWidth()
{
    return frameWidth;
}

int Frame::getHeight()
{
    return frameHeight;
}

bool Frame::canDeleteLayer()
{
    return layers.size() > 1;
}

Frame& Frame::operator=(const Frame& other)
{
    if (this == &other)
    {
        return *this;
    }

    for (Layer* layer : layers)
        delete layer;
    layers.clear();

    delete onionSkinLayer;
    onionSkinLayer = nullptr;

    frameWidth = other.frameWidth;
    frameHeight = other.frameHeight;

    for (Layer* layer : other.layers)
    {
        layers.push_back(new Layer(*layer));
    }

    activeLayerIndex = other.activeLayerIndex;
    activeLayer = layers[activeLayerIndex];

    return *this;
}

Layer Frame::composeFrame()
{
    Layer combo = Layer(frameWidth, frameHeight);
    for(Layer *l : layers)
    {
        const auto& pixels = l->getPixels();

        for(const auto& [position, color] : pixels)
        {
            combo.drawPixel(position, color);
        }
    }
    return combo;
}

std::unordered_map<Point2D, QColor> Frame::getPixels()
{
    return composeFrame().getPixels();
}

void Frame::selectLayer(const QModelIndex& index)
{
    if(!index.isValid())
    {
        return;
    }
    int idx = index.row();
    if(idx < 0 || idx >= layers.size())
    {
        return;
    }

    activeLayerIndex = idx;
    activeLayer = layers[idx];
}

void Frame::addLayer()
{
    Layer* newLayer = new Layer(frameWidth, frameHeight);
    layers.push_back(newLayer);

    int insertIndex = layers.size() - 1;

    activeLayerIndex = insertIndex;
    activeLayer = layers[activeLayerIndex];
}

void Frame::deleteLayer()
{
    if(layers.size() <= 1)
    {
        return;
    }

    int indexToRemove = activeLayerIndex;
    delete layers[indexToRemove];
    layers.erase(layers.begin() + indexToRemove);

    if(indexToRemove >= layers.size())
    {
        activeLayerIndex = layers.size()-1;
    }
    else
    {
        activeLayerIndex = indexToRemove;
    }
    activeLayer = layers[activeLayerIndex];
}

void Frame::reorderLayer(int destination, int origin)
{
    if (origin < 0 || origin >= layers.size())
    {
        return;
    }

    if (destination < 0 || destination > layers.size())
    {
        return;
    }

    Layer* temp = layers[origin];
    layers.erase(layers.begin() + origin);
    layers.insert(layers.begin() + destination, temp);

    activeLayerIndex = destination;
    activeLayer = layers[activeLayerIndex];
}

Layer* Frame::getLayer(int index)
{
    if(index < 0 || index >= layers.size())
    {
        return nullptr;
    }
    return layers[index];
}

std::unordered_map<Point2D, QColor> Frame::getActiveLayerPixels()
{
    if (!activeLayer) return {};
    return activeLayer->getPixels();
}

int Frame::layerCount()
{
    return layers.size();
}

int Frame::getActiveLayerIndex()
{
    return activeLayerIndex;
}

void Frame::resizeFrame(int newWidth, int newHeight) {
    frameWidth = newWidth;
    frameHeight = newHeight;

    for (Layer* layer : layers) {
        layer->resizeLayer(newWidth, newHeight);
    }
}
