/**
 * Date created: 3/23/2026
 * Authors: Francisco Pinas, Joshua Green
 *
 * This is the implementation of the canvas model, which is the backend for our pixel grid canvas.
 */


#include "canvas.h"
#include "qtimer.h"
#include <fstream>


Canvas::Canvas(int gridWidth, int gridHeight, QObject *parent)
    : QObject(parent)
    , frames{}
    , activeFrameIndex(0)
    , width(gridWidth)
    , height(gridHeight)

{
    addFrame();
    // Default pen status is draw.
    currentPenModeStatus = PenMode::draw;

    // Setting up a blank frame and some default values (there must always be one frame active).

    framerate = 60;

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Canvas::nextFrame);

    // Default penColor is red.
    penColor = QColor(255,0,0,255);
}

Canvas::~Canvas() {
    for (Frame* f : frames) {
        delete f;
    }
    frames.clear();
}

int Canvas::frameCount()
{
    return frames.size();
}

bool Canvas::canDelete()
{
    return frames.size() > 1;
}

int Canvas::getActiveFrameIndex()
{
    return activeFrameIndex;
}

void Canvas::startAnimation(int fps) {
    framerate = fps;
    int interval = 1000 / framerate;
    animationTimer->start(interval);
}

void Canvas::stopAnimation() {
    animationTimer->stop();
}

void Canvas::addFrame()
{
    Frame* newFrame = new Frame(width, height);
    frames.push_back(newFrame);

    int insertIndex = frames.size();

    activeFrameIndex = insertIndex - 1;
    activeFrame = frames[activeFrameIndex];

    updateOnionSkin();

    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit refreshFullCanvas(frames[activeFrameIndex]->activeLayer->getPixels());
    emit frameChanged(frames[activeFrameIndex]);
    emit frameSelected(activeFrameIndex);
}

void Canvas::deleteFrame() {
    if(frames.size() <= 1)
    {
        return;
    }

    int indexToRemove = activeFrameIndex;
    delete frames[indexToRemove];
    frames.erase(frames.begin() + indexToRemove);

    if(indexToRemove >= frames.size())
    {
        activeFrameIndex = frames.size()-1;
    }
    else
    {
        activeFrameIndex = indexToRemove;
    }

    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit refreshFullCanvas(frames[activeFrameIndex]->activeLayer->getPixels());
    emit frameChanged(frames[activeFrameIndex]);
    emit frameSelected(activeFrameIndex);
}

void Canvas::selectFrame(const QModelIndex& index) {
    if(!index.isValid())
    {
        return;
    }
    int idx = index.row();
    if(idx < 0 || idx >= frames.size())
    {
        return;
    }

    activeFrameIndex = idx;
    updateOnionSkin();
    emit frameSelected(activeFrameIndex);
    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit refreshFullCanvas(frames[activeFrameIndex]->activeLayer->getPixels());
}

void Canvas::duplicateFrame() {
    int insertIndex = activeFrameIndex + 1;

    Frame* duplicate = new Frame(*frames[activeFrameIndex]);
    frames.insert(frames.begin() + insertIndex, duplicate);

    activeFrameIndex = insertIndex;

    updateOnionSkin();

    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit refreshFullCanvas(frames[activeFrameIndex]->activeLayer->getPixels());
    emit frameChanged(frames[activeFrameIndex]);
    emit frameSelected(activeFrameIndex);
}

void Canvas::reorderFrame(int destination, int origin)
{
    if (origin < 0 || origin >= frames.size())
    {
        return;
    }

    if (destination < 0 || destination > frames.size())
    {
        return;
    }

    Frame* temp = frames[origin];
    frames.erase(frames.begin() + origin);
    frames.insert(frames.begin() + destination, temp);

    activeFrameIndex = destination;
    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit frameSelected(activeFrameIndex);
}

void Canvas::pixelClicked(int row, int col)
{
    switch(currentPenModeStatus)
    {
    case PenMode::draw:
    {
        // This ensures that when a selection is made, the user is restricted to that selection.
        if(frames[activeFrameIndex]->currentSelection.count(Point2D(row,col)) != 0 || (frames[activeFrameIndex]->currentSelection.empty())) {
            frames[activeFrameIndex]->activeLayer->drawPixel(Point2D(row, col), penColor);
            emit updatePixelData(Point2D(row, col), penColor);
        }
        break;
    }
    case PenMode::erase:
    {
        if(frames[activeFrameIndex]->currentSelection.count(Point2D(row,col)) != 0 || (frames[activeFrameIndex]->currentSelection.empty())) {
            frames[activeFrameIndex]->activeLayer->erasePixel(Point2D(row, col));
            emit erasePixelData(Point2D(row, col));
        }
        break;
    }
    case PenMode::fill:
    {
        if(frames[activeFrameIndex]->currentSelection.count(Point2D(row,col)) != 0 || (frames[activeFrameIndex]->currentSelection.empty())) {
            unordered_set<Point2D> allPixels = frames[activeFrameIndex]->activeLayer->fillPixel(Point2D(row, col), penColor);
            emit fillPixelArea(allPixels, penColor);
        }
        break;
    }
    case PenMode::shape_select:
    {
        frames[activeFrameIndex]->currentSelection = frames[activeFrameIndex]->activeLayer->shapeSelect(Point2D(row,col));
        emit updateSelection(frames[activeFrameIndex]->currentSelection);
        break;
    }
    case PenMode::color_shift:
    {
        if(frames[activeFrameIndex]->currentSelection.count(Point2D(row,col)) != 0 || (frames[activeFrameIndex]->currentSelection.empty())) {
            unordered_map<Point2D, QColor> shifted =
                frames[activeFrameIndex]->activeLayer->colorShift(Point2D(row, col), penColor);
            emit colorShiftArea(shifted);
        }
        break;
    }
    }
}
/**
 * @brief Canvas::nextFrame - to update to the next frame
 */
void Canvas::nextFrame(){
    if(frames.empty()){
        return;
    }

    // Using modulo to loop back to the first frame after the last one
    activeFrameIndex = (activeFrameIndex + 1) % frames.size();

    updateOnionSkin();

    // Notifying the UI that the selection has changed
    emit frameSelected(activeFrameIndex);

    // Get the pixel data from the newly active frame's layer
    // and send it to the CanvasWidget for a full redraw
    const auto& pixelMap = frames[activeFrameIndex]->getPixels();
    emit refreshFullCanvas(pixelMap);
}

Frame* Canvas::getFrame(int index)
{
    if(index < 0 || index >= frames.size())
    {
        return nullptr;
    }
    return frames[index];
}

void Canvas::pixelHovered(int row, int col) {
    emit hoveredOverPixel(row, col);
}

int Canvas::getWidth() const
{
    return width;
}

int Canvas::getHeight() const
{
    return height;
}

void Canvas::importFile(const std::string& filename) {
    std::ifstream loadFile(filename);
    if (!loadFile.is_open()) {
        qWarning() << "Failed to open file for reading:" << QString::fromStdString(filename);
        return;
    }

    json j;
    loadFile >> j;
    loadFile.close();

    from_json(j, *this);

    emit updateSelection(frames[activeFrameIndex]->currentSelection);
    emit refreshFullCanvas(frames[activeFrameIndex]->getPixels());
}

void Canvas::exportFile(const std::string& filename) {
    json j = *this;

    std::ofstream saveFile(filename);
    if (!saveFile.is_open()) {
        qWarning() << "Failed to open file for writing:" << QString::fromStdString(filename);
        return;
    }

    saveFile << j.dump(4);
    saveFile.close();
}

void Canvas::updateOnionSkin(){
    if(!onionSkinEnabled || activeFrameIndex <= 0 || frames.empty()){
        emit sendOnionSkin(std::unordered_map<Point2D, QColor>());
        return;
    }

    // Retrieve pixels from the frame immediately preceding the active one.
    std::unordered_map<Point2D, QColor> previousFramePixels = frames[activeFrameIndex - 1]->activeLayer->getPixels();

    emit sendOnionSkin(previousFramePixels);
}

void Canvas::toggleOnionSkin(bool enabled) {
    onionSkinEnabled = enabled;
    // Trigger a refresh so the skin appears/disappears immediately when clicked
    updateOnionSkin();
}

void Canvas::resizeCanvas(int newSize) {
    this->width = newSize;
    this->height = newSize;

    // tell every frame to resize its layers
    for (Frame* f : frames) {
        f->resizeFrame(newSize, newSize);
    }

    emit frameCountUpdated(frames.size());

    emit refreshFullCanvas(frames[activeFrameIndex]->activeLayer->getPixels());
}
void Canvas::setPenColor(const QColor &color) {
    penColor = color;
}

void Canvas::setFPS(int newFps) {
    if (newFps <= 0) return; // Prevent division by zero

    framerate = newFps;

    // If the timer is actively ticking, update its interval right now
    if (animationTimer->isActive()) {
        animationTimer->setInterval(1000 / framerate);
    }
}


