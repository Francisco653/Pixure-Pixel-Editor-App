/**
 * Date: 3/23/2026
 * Authors: Francisco Pinas, Joshua Green
 *
 * This header outlines all the necessary methods and fields for the canvas model.
 * The model of which holds data structures representing the pixel editor canvas, such as pixels drawn, current Frame, layers, etc.
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QModelIndex>
#include <QColor>
#include <QTimer>
#include <tuple>
#include <vector>
#include <string>
#include "frame.h"
#include "layer.h"
#include <json.hpp>
using json = nlohmann::json;


using std::vector;
using std::tuple;
using std::string;
using std::remove;

class Canvas : public QObject
{
    Q_OBJECT

    // Ensures that the JSON serializer can edit canvas object properly.
    friend void to_json(json& j, const Canvas& canvas);
    friend void from_json(const json& j, Canvas& canvas);

    enum class PenMode;
public:

    /**
     * @brief Canvas - Constructor for a canvas object (no default constructor).
     * @param width - The width which will determine the size of the pixel images.
     * @param height - The height which will determine the size of the pixel images.
     */
    Canvas(int gridWidth, int gridHeight, QObject *parent = nullptr);

    ~Canvas();

    /**
     * @brief frameCount - Returns the number of frames
     * @return int - the number of frames
     */
    int frameCount();

    /**
     * @brief getFrame - Returns the frame at the index
     * @param index - the location of the desired frame
     * @return Frame* - a pointer to the frame at the index. nullptr if there is no frame
     */
    Frame* getFrame(int index);

    /**
     * @brief getWidth - Returns the grid width of the canvas
     * @return int - the width
     */
    int getWidth() const;

    /**
     * @brief getHeight - Returns the grid height of the canvas
     * @return int - the height
     */
    int getHeight() const;

    /**
     * @brief addFrame - Adds a frame to the canvas
     */
    void addFrame();

    /**
     * @brief deleteFrame - Deletes the actively selected frame.
     */
    void deleteFrame();

    /**
     * @brief selectLayer - delegates to Frame method, allows for canvas ownership
     */
    void selectLayer(const QModelIndex& index);

    /**
     * @brief getActiveFrameIndex - Returns the index of the active frame
     * @return int - the active frame's index
     */
    int getActiveFrameIndex();

    /**
     * @brief resizeCanvas - the gernal canvas size
     * @param newSize - the new size we will allocate for the canvas(32 x32, 64 x 64, etc)
     */
    void resizeCanvas(int newSize);

    /**
     * @brief activeFrame - the active frame
     */
    Frame* activeFrame;

    /**
     * @brief startAnimation - starts the playback
     * @param fps - speed of animation
     */
    void startAnimation(int fps);

    /**
     * @brief stopAnimation - stops playback
     */
    void stopAnimation();

    /**
     * @brief setFPS - fps of animation
     * @param newFps - the speed of animation
     */
    void setFPS(int newFps);



private:
    // Represents the different modes the user can be on while pixel editing.
    enum class PenMode {
        draw, erase, fill, shape_select, color_shift
    };

    Q_ENUM(PenMode);

    vector<Frame*> frames;
    int framerate;
    int activeFrameIndex;
    PenMode currentPenModeStatus;

    QColor penColor;

    // Controlls playback timing
    QTimer* animationTimer;

    int width;
    int height;

    /**
     * @brief animate - Plays the users frames to show their animation.
     */
    void animate();

    /**
    * @brief updateOnionSkin - Logic to retrieve the previous frame's data.
    */
    void updateOnionSkin();

    // Meant for the UI
    bool onionSkinEnabled = false;

public slots:

    /**
     * @brief selectFrame - Selects the active frame (sets activeFrame).
     * @param frame - The frame that will become the active frame.
     */
    void selectFrame(const QModelIndex& index);

    /**
     * @brief duplicateFrame - Duplicates the actively selected frame.
     * @param frame - The frame which will be duplicated.
     */
    void duplicateFrame();

    /**
     * @brief reorderFrame - moves frame from one position in the frame list to another.
     * @param destination - the index where the frame is being placed
     * @param origin - the index where the frame originally came from
     */
    void reorderFrame(int destination, int origin);

    /**
     * @brief canDelete - Whether deletion for frames should be enabled
     * @return bool - if there is greater than 1 frames, true, otherwise false
     */
    bool canDelete();

    /**
     * @brief pixelClicked - When a pixel on the canvas is clicked
     *
     * Recieves signal that the pixel is clicked.
     * Send signal back to widget to draw pixel, do selection something, allow
     * the input of the user to be meaningful.
     *
     * @param row - the grid row of the clicked pixel (y coordinate)
     * @param col - the grid col of the clicked pixel (x coordinate)
     */
    void pixelClicked(int row, int col);

    /**
     * @brief pixelHovered - When a pixel is hovered over, we still need to update currentPixel.
     * @param row - the grid row of the clicked pixel (y coordinate)
     * @param col - the grid col of the clicked pixel (x coordinate)
     */
    void pixelHovered(int row, int col);

    /**
     * @brief nextFrame - Advances the animation by one frame
     */
    void nextFrame();

    /**
     * @brief toggleOnionSkin - when the button in the UI is clicked, activate onion skin function.
     * @param enabled - onion skinning of frames is visible.
     */
    void toggleOnionSkin(bool enabled);

    /**
     * @brief importFile - Imports a file, to load a project from our JSON data structure.
     * @param filename
     */
    void importFile(const string& filename);

    /**
     * @brief exportFile - Exports a file in a proprietry JSON structure for this app
     * @param filename
     */
    void exportFile(const string& filename);

    /**
     * @brief setPenModeDraw - sets the pen mode to draw
     */
    void setPenModeDraw(){
        currentPenModeStatus = PenMode::draw;
    }

    /**
     * @brief setPenModeErase - sets the pen mode to erase
     */
    void setPenModeErase(){
        currentPenModeStatus = PenMode::erase;
    }

    /**
     * @brief setPenModeFill - sets the pen mode to fill
     */
    void setPenModeFill(){
        currentPenModeStatus = PenMode::fill;
    }

    /**
     * @brief setPenModeShapeSelect - sets the pen mode to shape select
     */
    void setPenModeShapeSelect(){
        currentPenModeStatus = PenMode::shape_select;
    }

    /**
     * @brief setPenModeColorShift - sets the pen mode to color shift
     */
    void setPenModeColorShift() {
        currentPenModeStatus = PenMode::color_shift;
    }

    /**
     * @brief setPenColor - sets the color of the pen when selected on the color picker
     */
    void setPenColor(const QColor &color);

signals:
    /**
     * @brief frameSelected - emits when a frame is selected in the ui (by clicking item in list view)
     * @param index - the index of the selected frame within the vector
     */
    void frameSelected(int index);

    /**
     * @brief frameChanged - the new active frame
     * @param frame - the new active frame
     */
    void frameChanged(Frame* frame);

    /**
     * @brief layerSelected - emits when a layer is selected in the ui
     * @param index - the index of the selected layer from within the layers vector of the active frame
     */
    void layerSelected(int index);

    /**
     * @brief refreshFullCanvas - sends all pixels at once so the UI can swap frames instantly
     * @param pixels - current pixels in the frame
     */
    void refreshFullCanvas(const std::unordered_map<Point2D, QColor>& pixels);

    /**
     * @brief frameCountUpdated - emits when vector length changes (adding, duplicating, deleting)
     * @param numOfFrames - the up-to-date value representing the number of frames in the vector
     */
    void frameCountUpdated(int numOfFrames);

    /**
     * @brief updatePixelData - Communicates to CanvasWidget to update a specific pixel in the view.
     * @param pixel - The pixel being updated.
     * @param color - The color the pixel was changed to.
     */
    void updatePixelData(Point2D pixel, QColor color);

    /**
     * @brief erasePixelData - Communicates to CanvasWidget that a pixel has been erased.
     * @param pixel - The pixel being erased.
     */
    void erasePixelData(Point2D pixel);

    /**
     * @brief updateSelection - Communicates to CanvasWidget to update the current pixel(s) selection in the view.
     * @param selection
     */
    void updateSelection(unordered_set<Point2D> selection);

    /**
     * @brief fillPixelArea - Communicates to CanvasWidget to update the selection of pixels to the new fill color.
     * @param selection - The selection/area of pixels which need to be filled
     * @param color - The color to fill with.
     */
    void fillPixelArea(unordered_set<Point2D> filled, QColor color);

    /**
     * @brief colorShiftArea - Communicates to CanvasWidget to update the selection of pixels affected by the color shift.
     * @param shifted - The color shifted area of pixels
     */
    void colorShiftArea(unordered_map<Point2D, QColor> shifted);

    /**
     * @brief frameRemoved - Signals to the frame list that a frame has been removed
     * @param index - the index where the frame was removed
     */
    void frameRemoved(int index);

    /**
     * @brief frameInserted - Signals to the frame list that a frame has been inserted
     * @param index - the index where the frame was inserted
     */
    void frameInserted(int index);

    /**
     * @brief hoveredOverPixel - Signals to the view that a pixel has been hovered over
     * @param row - the row of the hovered pixel
     * @param col - the col of the hovered pixel
     */
    void hoveredOverPixel(int row, int col);

    /**
     * @brief sendOnionSkin - Sends pixel data from the previous frame to be drawn faintly.
     */
    void sendOnionSkin(const std::unordered_map<Point2D, QColor>& pixels);

    void layerInserted(int index);
};

#endif // CANVAS_H
