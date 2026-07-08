#include "previewwindow.h"
#include <QPainter>

PreviewWindow::PreviewWindow(Canvas* canvasPtr, QWidget *parent)
    : QWidget(parent)
    , canvas(canvasPtr) // Initialize the pointer
{
    setWindowTitle("Preview");
    setWindowFlags(Qt::Window); // Makes it a separate popup window
}

void PreviewWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    // "Accessing something" directly:
    // We get the size straight from the canvas pointer
    int w = canvas->getWidth();
    int h = canvas->getHeight();

    // Set the window to the true size of the canvas
    if (this->size() != QSize(w, h)) {
        this->setFixedSize(w, h);
    }

    // Draw the pixels 1:1
    for (auto const& [point, color] : currentPixels) {
        painter.fillRect(point.y, point.x, 1, 1, color);
    }
}

void PreviewWindow::updatePreview(const std::unordered_map<Point2D, QColor>& pixels) {
    currentPixels = pixels;
    update();
}
