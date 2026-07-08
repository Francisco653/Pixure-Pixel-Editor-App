/**
 * Date:
 * Authors:
 */

#ifndef LAYERLISTMODEL_H
#define LAYERLISTMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include <QPixmap>
#include "frame.h"

class LayerListModel  : public QAbstractListModel
{
    Q_OBJECT
public:
    LayerListModel(Frame* frame, QObject* parent = nullptr);

    /**
     * @brief rowCount - the number of layers in the list
     * @return int - the number of layers in the list
     */
    int rowCount(const QModelIndex&) const override;

    /**
     * @brief setFrame - to update the frame selection
     * @param newFrame - the new active Frame
     */
    void setFrame(Frame* newFrame);

public slots:
    /**
     * @brief data - what is shown for each layer in the QListView
     * @param index - the position in the vector of the layers
     * @param role - the kind of data being requested
     * @return QVariant - the data to display based on the layer's info
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * @brief supportedDropActions - returns the Qt::MoveAction to allow drag-and-drop reordering
     * @return Qt::MoveAction to allow drag-and-drop reordering
     */
    Qt::DropActions supportedDropActions() const override;

    /**
     * @brief flags - Defines the permitted actions for items in the frames list
     * @param index - the position of a frame in the vector
     * @return Qt::ItemFlags - a set of flags (bools) that describe the item's capabilities
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief moveRows - allows for layer reordering
     * @param originalIndex - the original index of the dragged layer
     * @param count - not used
     * @param destinationIndex - the destination index of the dragged layer
     * @return bool - true if the move is successful, false otherwise
     */
    bool moveRows(const QModelIndex &, int originalIndex, int count, const QModelIndex &, int destinationIndex) override;

    /**
     * @brief appendLayer - Appends a layer to the end of the list
     */
    void appendLayer();

    /**
     * @brief removeLayer - Removes the selected layer from the list
     */
    void removeLayer();

    /**
     * @brief updateCurrentLayer - Updates the thumbnail of the currently active frame in the layer list.
     *
     * This is called whenever the active layer's pixel data changes (by drawing).
     * It tells Qt that only this specific row has changed so the thumbnail
     * can be redrawn without refreshing every pixel of every layer in the entire list at once (would be super slow).
     */
    void updateCurrentLayer();

    /**
    * @brief refreshList - Emits the signal that tells the view to update what is being displayed
    *
    * Ensures the UI is up-to-date with the stored layer data (used when switching frames or reloading data)
    */
    void refreshList();

    /**
     * @brief frameSelectionChanged - changes the frame shown in the model
     * @param frame - the new frame
     */
    void frameSelectionChanged(Frame* frame);

private:
    Frame* frame;

};

#endif // LAYERLISTMODEL_H
