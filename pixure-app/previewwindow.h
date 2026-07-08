#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QWidget>
#include "point2d.h"
#include <QColor>
#include <unordered_map>
#include "canvas.h"

/**
 * @brief PreviewWindow::PreviewWindow - Constructor for the preview window.
 * @param canvasPtr - A pointer to the main Canvas model to access dimensions.
 * @param parent - The parent QWidget (usually MainWindow).
 */
class PreviewWindow : public QWidget
{
    Q_OBJECT
public:
    // Ensure the constructor is defined like this:
    explicit PreviewWindow(Canvas* canvasPtr, QWidget *parent = nullptr);

    /**
    * @brief PreviewWindow::updatePreview - Updates the internal pixel map and triggers a redraw.
    * @param pixels - The current frame's pixel data from the Canvas model.
    */
    void updatePreview(const std::unordered_map<Point2D, QColor>& pixels);

protected:
    /**
    * @brief PreviewWindow::paintEvent - Draws the pixel data at 1:1 scale.
    * @param event - The paint event triggered by update().
    */
    void paintEvent(QPaintEvent *event) override;

private:
    Canvas* canvas;
    std::unordered_map<Point2D, QColor> currentPixels;
};

#endif
