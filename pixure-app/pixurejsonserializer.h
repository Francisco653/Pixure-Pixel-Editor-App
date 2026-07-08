/**
 * Date: 3/29/2026
 * Authors: Francisco Pinas
 * Credits to: Niels Lohmann for the JSON library: https://github.com/nlohmann/json
 *
 * This code serializes/deserializes the different data structures of the pixure app to allow for saving and loading projects.
 * The files will be saved in .ssp extension.
 */

#ifndef PIXUREJSONSERIALIZER_H
#define PIXUREJSONSERIALIZER_H


#include "canvas.h"
#include "frame.h"
#include "layer.h"
#include "point2d.h"

#include <json.hpp>
using json = nlohmann::json;

/**
 * @brief to_json - Serializes canvas objects, which is used for saving pixure projects.
 * @param j - The JSON serializer.
 * @param canvas - The canvas model being serialized.
 */
void to_json(json& j, const Canvas& canvas);

/**
 * @brief from_json - Deserializes canvas model, which is used for loading pixure projects.
 * @param j - The JSON deserializer
 * @param canvas - The canvas model being loaded into the project.
 */
void from_json(const json& j, Canvas& canvas);

/**
 * @brief to_json - Serializes frame objects, part of the canvas model.
 * @param j - The JSON serializer.
 * @param frame - The frame model being serialized.
 */
void to_json(json& j, const Frame& frame);

/**
 * @brief from_json - Deserializes frame object.
 * @param j - The JSON deserializer.
 * @param frame - The frame model being deserialized.
 */
void from_json(const json& j, Frame& frame);

/**
 * @brief to_json - Serializes layer objects, part of the canvas model.
 * @param j - The JSON serializer.
 * @param layer - The layer model being serialized.
 */
void to_json(json& j, const Layer& layer);

/**
 * @brief from_json - Deserializes layer object.
 * @param j - The JSON deserializer.
 * @param frame - The layer model being deserialized.
 */
void from_json(const json& j, Layer& layer);

/**
 * @brief to_json - Serializes point2D objects, part of the canvas model.
 * @param j - The JSON serializer.
 * @param point - The Point2D pixel model being serialized.
 */
void to_json(json& j, const Point2D& point);

/**
 * @brief from_json - Deserializes point2D object.
 * @param j - The JSON deserializer.
 * @param frame - The Point2D model being deserialized.
 */
void from_json(const json& j, Point2D& point);

/**
 * @brief to_json - Serializes the pixels array within Layers class.
 * @param j - The JSON serializer.
 * @param pixels - A map holding the drawn pixels and their color.
 */
void to_json(json& j, const unordered_map<Point2D, QColor>& pixels);

/**
 * @brief from_json - Deserializes the pixels array.
 * @param j - The JSON deserializer
 * @param pixels - A map holding the drawn pixels and their color.
 */
void from_json(const json& j, unordered_map<Point2D, QColor>& pixels);

#endif // PIXUREJSONSERIALIZER_H
