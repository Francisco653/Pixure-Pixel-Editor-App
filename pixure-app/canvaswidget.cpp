/**
 * Date created: 3/25/2026
 * Author(s): Francisco Pinas, Sebastian Flores.
 *
 * Implementation of canvas widget, which draws out the pixels for the pixel editor on a grid canvas.
 */

#include "CanvasWidget.h"
#include <QPainter>
#include <QMouseEvent>


CanvasWidget::CanvasWidget(QWidget* parent)
    : CanvasWidget(16, 16, parent)
{
    // this->setMouseTracking(true);
    // hoveredPixel = QPoint(-1, -1);
}

CanvasWidget::CanvasWidget(int rows, int cols, QWidget *parent)
    : QWidget(parent),
    gridRows(rows),
    gridColumns(cols),

    // Initializes the background to be white to start.
    cells(gridRows, std::vector<QColor>(gridColumns, QColor(255,255,255, 10)))
{
    this->setMouseTracking(true);
    hoveredPixel = QPoint(-1, -1);
}

QSize CanvasWidget::sizeHint() const
{
    // A reasonable starting size so the widget is visible in Designer/layouts
    return QSize(gridColumns * 20, gridRows * 20);
}


void CanvasWidget::paintEvent(QPaintEvent *)
{
    // Setting up painter and different pen types
    QPainter painter(this);
    QPen gridPen = QPen(Qt::gray);
    QPen selectionPen = QPen(Qt::DashLine);
    painter.setPen(gridPen);

    // Compute cell size based on current widget size
    cellSize = std::min(width() / gridColumns, height() / gridRows);
    if (cellSize <= 0)
        return; //

    for (int r = 0; r < gridRows; ++r) {
        for (int c = 0; c < gridColumns; ++c) {

            // Compute the rectangle for this cell on screen
            QRect cell(c * cellSize, r * cellSize, cellSize, cellSize);
            painter.fillRect(c * cellSize, r * cellSize, cellSize, cellSize, cells[r][c]);

            painter.setPen(Qt::gray);
            // Draw the grid outline
            painter.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);
        }
    }

    // Drawing the onion skin
    // looping through our stored "ghost" pixels and draw them with transparency
    for(auto const& [point, color] : onionSkinPixels){
        QColor ghostColor = color;
        ghostColor.setAlpha(80); // Set transparency (0 is invisible, 255 is solid)

        painter.fillRect(point.y * cellSize, point.x * cellSize, cellSize, cellSize, ghostColor);
    }

    for (int r = 0; r < gridRows; ++r) {
        for (int c = 0; c < gridColumns; ++c) {

            // Compute the rectangle for this cell on screen
            QRect cell(c * cellSize, r * cellSize, cellSize, cellSize);

            if(cells[r][c] != Qt::white){
                // Fill the cell with its stored color
                painter.fillRect(c * cellSize, r * cellSize, cellSize, cellSize, cells[r][c]);
            }

            if (showGridLines) {
                painter.setPen(Qt::gray);
                painter.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);
            }
        }
    }
    // Redraw pixels that are selected to have the appropriate dashed border
    painter.setPen(selectionPen);
    for (const QPoint &pt : selectedCells) {

        // We need to invert x and y here to draw correctly, as Qpoint is flipped to our 2D point).
        QRect cell(pt.y() * cellSize, pt.x() * cellSize, cellSize, cellSize);
        painter.drawRect(cell);
    }

    if (hoveredPixel.x() >= 0 && hoveredPixel.y() >= 0)
    {
        QPainter painter(this);

        painter.setPen(QPen(Qt::blue, 2));

        int x = hoveredPixel.x() * cellSize;
        int y = hoveredPixel.y() * cellSize;

        // This check ensures we don't highlight out of bounds.
        if(x <= (gridRows -1) * cellSize && y <= (gridColumns -1) * cellSize)
        painter.drawRect(x, y, cellSize, cellSize);
    }
}

void CanvasWidget::setPixelAt(int row, int col, QColor color)
{
    // Ignore anything outside the grid.
    if (row < 0 || row >= gridRows || col < 0 || col >= gridColumns)
        return;

    cells[row][col] = color;

    // Request a repaint so the change appears on screen
    update();
}


void CanvasWidget::mousePressEvent(QMouseEvent *e)
{
    // Only left click should draw.
    if(e->buttons() & Qt::LeftButton) {
    int col = e->pos().x() / cellSize;
    int row = e->pos().y() / cellSize;
    emit clickAtPixel(row,col);
    }
}


void CanvasWidget::mouseMoveEvent(QMouseEvent *e)
{
    // Neede to recalculate cellSize beased on the current width/height and grid size.
    cellSize = std::min(width() / gridColumns, height() / gridRows);
    int col = e->pos().x() / cellSize;
    int row = e->pos().y() / cellSize;

    hoveredPixel = QPoint(col, row);
    update();

    // Only emit if the mouse is actually inside the grid boundaries
    if(row >= 0 && row < gridRows && col >= 0 && col < gridColumns){
        emit sendMousePosition(row, col);
    }
    else{
        // Tell the UI to show the "empty" state
        emit sendMousePosition(row, col);
    }

    // Handles dragging the mouse
    if (e->buttons() & Qt::LeftButton) {

        emit clickAtPixel(row,col);
    }
}

void CanvasWidget::updatedPixel(Point2D point, QColor color) {
    setPixelAt(point.x,point.y, color);
}

void CanvasWidget::initializeGrid(int rows, int cols) {
    gridRows = rows;
    gridColumns = cols;
    cells.assign(rows, std::vector<QColor>(cols, QColor(255,255,255, 10)));
    hoveredPixel = QPoint(-1, -1);
    update();
}

void CanvasWidget::showSelection(unordered_set<Point2D> selectedPixels) {
    selectedCells.clear();
    for(Point2D pixel : selectedPixels) {
        selectedCells.insert(QPoint(pixel.x,pixel.y));
        update();
    }
}
void CanvasWidget::onRefreshFullCanvas(const std::unordered_map<Point2D, QColor>& pixels){

    //Clear the entire grid
    for(int r = 0; r < gridRows; r++){
        for(int c = 0; c < gridColumns; c++){
            cells[r][c] = QColor(255,255,255, 10);
        }
    }

    // Overlay the pixels from the provided map
    for(auto const& [point, color] : pixels){
        if(point.x >= 0 && point.x < gridRows && point.y >= 0 && point.y < gridColumns){
            cells[point.x][point.y] = color;
        }
    }


    // To trigger an update on the widget
    update();
}

void CanvasWidget::erasedPixel(Point2D point) {
    cells[point.x][point.y] = QColor(255,255,255, 10);
    update();
}

void CanvasWidget::filledPixelArea(unordered_set<Point2D> selection, QColor color) {
    for(Point2D pixel : selection) {
        cells[pixel.x][pixel.y] = color;
    }
    update();
}

void CanvasWidget::colorShiftedArea(unordered_map<Point2D, QColor> shifted) {
    for (const auto& [pixel, color] : shifted) {
        cells[pixel.x][pixel.y] = color;
    }
    update();
}

void CanvasWidget::hoveredOverPixel(int row, int col)
{
    hoveredPixel = QPoint(col, row);
    update();
}

void CanvasWidget::onReceiveOnionSkin(const std::unordered_map<Point2D, QColor>& pixels){
    this->onionSkinPixels = pixels;
    update();
}

void CanvasWidget::resizeGrid(int newRows, int newCols) {
    gridRows = newRows;
    gridColumns = newCols;

    //Resize the 2D vector and fill with white
    cells.assign(gridRows, std::vector<QColor>(gridColumns, QColor(255,255,255, 10)));

    hoveredPixel = QPoint(-1, -1);
    selectedCells.clear();

    //Trigger a redraw
    update();
}

void CanvasWidget::setGridVisible(bool visible) {
    showGridLines = visible;
    update();
}
