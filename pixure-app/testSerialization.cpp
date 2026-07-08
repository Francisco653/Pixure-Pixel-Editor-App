#include <iostream>
#include <json.hpp>
#include "canvas.h"
#include "frame.h"
#include "layer.h"


using json = nlohmann::json;

int main() {
    // 1. Build a tiny canvas
    Canvas original(4, 4);
    original.addFrame();
    Frame* f = original.getFrame(0);
    Layer* layer = f->layers[0];

    // Put a pixel somewhere
    layer->pixels[{1, 2}] = QColor("#FF00FF");

    // 2. Serialize
    json j = original;

    std::cout << "Serialized JSON:\n" << j.dump(4) << "\n";
    // 3. Deserialize into a NEW canvas
    Canvas loaded(4, 4);   // must match constructor requirements
    from_json(j, loaded);

    // 4. Compare
    auto& p1 = original.getFrame(0)->layers[0]->pixels;
    auto& p2 = loaded.getFrame(0)->layers[0]->pixels;

    if (p1 == p2)
        std::cout << "SUCCESS: Pixel maps match\n";
    else
        std::cout << "FAIL: Pixel maps differ\n";

    return 0;
}
