/**
 * Date: 3/25/26
 * Authors: Joshua Green
 * Docs URL: https://doc.qt.io/qt-6/qabstractlistmodel.html
 *
 * This class wires data from canvas to QListView in UI.
 *
 *
 */

#ifndef FRAMELISTMODEL_H
#define FRAMELISTMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include <QPixmap>
#include "canvas.h"

class FrameListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief FrameListModel - wires data from canvas to QListView in UI
     *
     * Reads from the vector of frames stored in Canvas, gives needed info to the view
     *
     * @param canvas - pointer to the canvas object of the app
     * @param parent - qt requires this parameter, not used here
     */
    FrameListModel(Canvas* canvas, QObject* parent = nullptr);

    /**
     * @brief rowCount - returns how many frames there are in the project
     *
     * Number of items displayed by QListView
     *
     * @param parent - does nothing
     * @return int - the number of frames in the project
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

public slots:
    /**
     * @brief data - what is shown for each frame in the QListView
     * @param index - the position in the vector of the frame
     * @param role - the kind of data being requested. Whether its the text label, the frame preview image, etc.
     * @return QVariant - the data to display based on the frame's info
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * @brief supportedDropActions - returns the Qt::MoveAction to allow drag-and-drop reordering
     * @return Qt::MoveAction to allow drag-and-drop reordering
     */
    Qt::DropActions supportedDropActions() const override;

    /**
     * @brief refreshList - Emist the signal that tells the view to update what is being displayed
     *
     * Ensures the UI is up-to-date with the stored frame data
     */
    void refreshList();

    /**
     * @brief flags - Defines the permitted actions for items in the frames list
     * @param index - the position of a frame in the vector
     * @return Qt::ItemFlags - a set of flags (bools) that describe the item's capabilities
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief moveRows - allows for frame reordering
     * @param originalIndex - the original index of the dragged frame
     * @param count - not used
     * @param destinationIndex - the destination index of the dragged frame
     * @return bool - true if the move is successful, false otherwise
     */
    bool moveRows(const QModelIndex &, int originalIndex, int count, const QModelIndex &, int destinationIndex) override;

    /**
     * @brief appendFrame - Appends a frame to the end of the list
     */
    void appendFrame();

    /**
     * @brief removeFrame - Removes the selected frame from the list
     */
    void removeFrame();

    /**
     * @brief duplicateFrame - Duplicates the selected frame
     */
    void duplicateFrame();

    /**
     * @brief updateCurrentFrame - Updates the thumbnail of the currently active frame in the frame list.
     *
     * This is called whenever the active frame’s pixel data changes (by drawing).
     * It tells Qt that only this specific row has changed so the thumbnail
     * can be redrawn without refreshing every pixel of every frame in the entire list at once (would be super slow).
     */
    void updateCurrentFrame();

private:
    Canvas* canvas;
};

#endif // FRAMELISTMODEL_H
