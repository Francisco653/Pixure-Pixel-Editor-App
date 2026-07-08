/**
 * Date: 3/25/2026
 * Author(s): Francisco Pinas
 *
 * This is the header for the canvas widget, the actual GUI representation of the pixel editor canvas.
 */

#pragma once
#include <QWidget>
#include <vector>
#include "point2d.h"

using std::unordered_set;
using std::unordered_map;

/**
 * @class CanvasWidget
 * @brief A simple pixel-grid drawing widget.
 *
 * CanvasWidget displays a fixed-size grid where each cell represents a
 * logical pixel. The user can draw by clicking or dragging the mouse,
 * and each affected cell is filled with a color.
 *
 * This widget is intended as a minimal foundation for a pixel editor.
 */
class CanvasWidget : public QWidget
{
    Q_OBJECT
public:

    /**
     * @brief CanvasWidget - This is the constructor called by default with Qt when using the form editor widget.
     * @param parent - QWidget inheritance.
     */
    CanvasWidget(QWidget* parent = nullptr);

    /**
     * @brief CanvasWidget - The visual representation for the canvas model, shows all the pixels and colors.
     *  This constructor is called by the default constructor above.
     * @param rows - The number of rows our pixel grid will have
     * @param cols - The number of columns our pixel grid will have
     * @param parent - QWidget inheritance.
     */
    CanvasWidget(int rows, int cols, QWidget *parent = nullptr);

    /**
     * @brief initializeGrid - Sets up a grid size after construction
     * @param rows
     * @param cols
     */
    void initializeGrid(int rows, int cols);

    /**
     * @brief resizeGrid - will chnage the size of the canvas
     * @param newRows - updated row size
     * @param newCols - updated col size
     */
    void resizeGrid(int newRows, int newCols);

    void setGridVisible(bool visible);


protected:
    /**
     * @brief Paints the pixel grid and its contents.
     *
     * Called automatically whenever the widget needs to be redrawn.
     * Draws each cell's color and outlines the grid.
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief Handles mouse press events.
     *
     * Converts the mouse position into a grid coordinate and fills
     * the corresponding cell.
     */
    void mousePressEvent(QMouseEvent *) override;

    /**
     * @brief Handles mouse movement while a button is pressed.
     *
     * Allows the user to draw continuously by dragging the mouse.
     */
    void mouseMoveEvent(QMouseEvent *) override;


signals:
    void clickAtPixel(int row, int col);
    void dragAcrossPixel(int row, int col);
    void sendMousePosition(int row, int col);

public slots:

    // Slot to recieve the entire frame's data
    void onRefreshFullCanvas(const std::unordered_map<Point2D, QColor>& pixels);

    /**
     * @brief onReceiveOnionSkin - slot to receive the onion skins data
     * @param pixels - the pixels stored in that frame
     */
    void onReceiveOnionSkin(const std::unordered_map<Point2D, QColor>& pixels);
    /**
     * @brief updatedPixel - Updates the pixel in the widget to its new color.
     * @param point - The pixel which is to be updated.
     * @param color - The color pixel will be set to.
     */
    void updatedPixel(Point2D point, QColor color);

    /**
     * @brief showSelection - Shows the current user selection from shape select tool.
     * @param selection - The pixels selected.
     */
    void showSelection(unordered_set<Point2D> selection);

    /**
     * @brief erasedPixel - Erases the specified pixel.
     * @param point - The pixel to erase.
     */
    void erasedPixel(Point2D point);

    /**
     * @brief filledPixelArea - Fills the pixel area with a certain color.
     * @param selection - The pixels to be filled.
     * @param color  - The color to fill with.
     */
    void filledPixelArea(unordered_set<Point2D> selection, QColor color);

    /**
     * @brief colorShiftedArea - Shifts the pixel area towards a certain color.
     * @param shifted - The shifted area of pixels
     */
    void colorShiftedArea(unordered_map<Point2D, QColor> shifted);

    /**
     * @brief updateHoveredPixel - Recieves position of the pixel being hovered over
     * @param row - the row position of the pixel
     * @param col - the col position of the pixel
     */
    void hoveredOverPixel(int row, int col);
private:
    int gridRows;       ///Number of rows in the pixel grid.
    int gridColumns;     ///Number of columns in the pixel grid.
    int cellSize;   ///Size of each cell in screen pixels.
    bool showGridLines = true;

    std::unordered_map<Point2D, QColor> onionSkinPixels; /// Stores the onion skins pixels

    // Stored the selected cells that will have to be redrawn.
    QSet<QPoint> selectedCells;

    // hovered pixel
    QPoint hoveredPixel;

    /**
     * @brief 2D array storing the color of each pixel cell.
     */
    std::vector<std::vector<QColor>> cells;

    /**
     * @brief A helper that sets the color of a specific pixel cell.
     * @param row Row index of the cell.
     * @param col Column index of the cell.
     * @param color The color to apply.
     *
     * Performs bounds checking and triggers a repaint.
     */

    void setPixelAt(int row, int col, QColor color);

    /**
     * @brief sizeHint - Overrides a QWidget method to make sure our widget is constructed at a reasonable size
     * @return
     */
    QSize sizeHint() const override;
};

