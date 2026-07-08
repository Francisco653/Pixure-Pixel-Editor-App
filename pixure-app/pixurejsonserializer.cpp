/**
 * Date:
 * Authors:
 */

#include "pixurejsonserializer.h"
#include <json.hpp>
using json = nlohmann::json;

// --- Canvas ---
void to_json(json& j, const Canvas& canvas) {

    // Dereferences the frames, as JSON doesn't like pointers much.
    json framesJson = json::array();
    for (Frame* frame : canvas.frames) {
        framesJson.push_back(*frame);
    }

    j = json{ {"frames", framesJson},
            {"height", canvas.getHeight()},
            {"width", canvas.getWidth()}
        };
}

void from_json(const json& j, Canvas& canvas) {
    j.at("width").get_to(canvas.width);
    j.at("height").get_to(canvas.height);

    for (Frame* f : canvas.frames)
        delete f;
    canvas.frames.clear();

    // Rebuild the frames list (due to not being able to carry pointers through JSON).
    for (const auto& frameJson : j.at("frames")) {
        Frame* f = new Frame(canvas.width, canvas.height);
        from_json(frameJson, *f);
        canvas.frames.push_back(f);
    }
    canvas.activeFrameIndex = 0;
}


// --- Frame ---
void to_json(json& j, const Frame& frame) {

    json layersJson = json::array();
    for (Layer* layer : frame.layers) {
        layersJson.push_back(*layer);
    }

    j = json{ {"layers", layersJson},
        {"height", frame.frameHeight},
        {"width", frame.frameWidth}
    };
}

void from_json(const json& j, Frame& frame) {

    j.at("width").get_to(frame.frameWidth);
    j.at("height").get_to(frame.frameHeight);

    for (Layer* layer : frame.layers)
        delete layer;
    frame.layers.clear();

    for (const auto& layerJson : j.at("layers")) {
        Layer* layer = new Layer(frame.frameWidth, frame.frameHeight);
        from_json(layerJson, *layer);
        frame.layers.push_back(layer);
    }

    // Reset the active layer and onion skin layers to avoid referencing any deleted memory addresses.
    frame.activeLayer = frame.layers.at(0);
    frame.onionSkinLayer = nullptr;
}

// --- Layer ---

void to_json(json& j, const Layer& layer) {

    j = json{
            {"pixels", layer.pixels},
            {"height", layer.layerHeight},
            {"width", layer.layerWidth}
    };
}

void from_json(const json& j, Layer& layer) {
    j.at("height").get_to(layer.layerHeight);
    j.at("width").get_to(layer.layerWidth);
    j.at("pixels").get_to(layer.pixels);
}


// --- Point2D ---

void to_json(json& j, const Point2D& point) {
    j = json{
        {"x", point.x},
        {"y", point.y}
    };
}

void from_json(const json& j, Point2D& point) {
    j.at("x").get_to(point.x);
    j.at("y").get_to(point.y);
}

// --- Pixels array within Layer class ---

void to_json(json& j, const unordered_map<Point2D, QColor>& pixels) {
    j = json::array();

    for (const auto& [point, color] : pixels) {
        j.push_back({
            {"point", point},
            {"color", color.name(QColor::HexRgb).toStdString()}
        });
    }
}

void from_json(const json& j, unordered_map<Point2D, QColor>& pixels) {
    pixels.clear();

    for (const auto& entry : j) {
        Point2D point = entry.at("point").get<Point2D>();

        QColor color(QString::fromStdString(
            entry.at("color").get<std::string>()
            ));

        pixels[point] = color;
    }
}
