#include <iostream>
#include <json.hpp>
#include "canvas.h"
#include "frame.h"
#include "layer.h"
#include "testHelper.h"

using json = nlohmann::json;

int main() {
    // 1. Build a tiny canvas
    Canvas original(4, 4);
    original.addFrame();

    Frame* f = CanvasTestHelper::frames(original)[0];
    Layer* layer = FrameTestHelper::layers(f)[0];

    // Put a pixel somewhere
    LayerTestHelper::pixels(layer)[{1, 2}] = QColor("#FF00FF");

    // 2. Serialize
    json j = original;
    std::cout << "Serialized JSON:\n" << j.dump(4) << "\n";

    // 3. Deserialize into a NEW canvas
    Canvas loaded(4, 4);
    from_json(j, loaded);

    // 4. Compare pixel maps
    auto& p1 = LayerTestHelper::pixels(
        FrameTestHelper::layers(
            CanvasTestHelper::frames(original)[0]
            )[0]
        );

    auto& p2 = LayerTestHelper::pixels(
        FrameTestHelper::layers(
            CanvasTestHelper::frames(loaded)[0]
            )[0]
        );

    if (p1 == p2)
        std::cout << "SUCCESS: Pixel maps match\n";
    else
        std::cout << "FAIL: Pixel maps differ\n";

    return 0;
}
