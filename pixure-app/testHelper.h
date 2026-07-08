#ifndef TESTHELPER_H
#define TESTHELPER_H

#include "canvas.h"
#include "frame.h"
#include "layer.h"

class LayerTestHelper {
public:
    static unordered_map<Point2D, QColor>& pixels(Layer* layer) {
        return layer->pixels;
    }
};

class FrameTestHelper {
public:
    static vector<Layer*>& layers(Frame* frame) {
        return frame->layers;
    }
};

class CanvasTestHelper {
public:
    static vector<Frame*>& frames(Canvas& canvas) {
        return canvas.frames;
    }
};

#endif
