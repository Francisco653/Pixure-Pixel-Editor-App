/**
 * Date:
 * Authors:
 */

#include "layerlistmodel.h"

LayerListModel::LayerListModel(Frame* frame, QObject* parent)
    : QAbstractListModel(parent)
    , frame(frame){
}

void LayerListModel::setFrame(Frame* newFrame)
{
    beginResetModel();

    frame = newFrame;

    endResetModel();
}

void LayerListModel::appendLayer()
{
    int idx = frame->layerCount();

    beginInsertRows(QModelIndex(), idx, idx);

    frame->addLayer();

    endInsertRows();
}

void LayerListModel::removeLayer()
{
    int idx = frame->getActiveLayerIndex();

    beginRemoveRows(QModelIndex(), idx, idx);

    frame->deleteLayer();

    endRemoveRows();
}

int LayerListModel::rowCount(const QModelIndex&) const {
    if (!frame) return 0;
    return frame->layerCount();
}

QVariant LayerListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    int itemIndex = index.row();
    Layer* layer = frame->getLayer(itemIndex);
    if (!layer)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        //return QString("Layer %1").arg(itemIndex);
    }

    if (role == Qt::DecorationRole)
    {
        auto pixels = layer->getPixels();

        int w = frame->getWidth();
        int h = frame->getHeight();

        QImage image(w, h, QImage::Format_ARGB32);
        image.fill(Qt::white);

        for (const auto& [position, color] : pixels)
        {
            if (position.x >= 0 && position.x < w && position.y >= 0 && position.y < h)
            {
                image.setPixelColor(position.y, position.x, color);
            }
        }

        //scales down to 64x64
        QPixmap pixmap = QPixmap::fromImage(image).scaled(64, 64, Qt::KeepAspectRatio);

        return pixmap;
    }

    return QVariant();
}

Qt::DropActions LayerListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void LayerListModel::updateCurrentLayer()
{
    if (!frame) return;
    int row = frame->getActiveLayerIndex();
    QModelIndex idx = index(row, 0);
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}

Qt::ItemFlags LayerListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if(index.isValid())
    {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    else
    {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

bool LayerListModel::moveRows(const QModelIndex &, int originalIndex, int count, const QModelIndex &, int destinationIndex)
{
    if (count != 1) return false;

    if (originalIndex == destinationIndex || originalIndex + 1 == destinationIndex)
    {
        return false;
    }

    beginMoveRows(QModelIndex(), originalIndex, originalIndex, QModelIndex(), destinationIndex);

    int itemIndex = destinationIndex;

    if(destinationIndex > originalIndex)
    {
        itemIndex--;
    }

    frame->reorderLayer(itemIndex, originalIndex);
    frame->selectLayer(index(itemIndex, 0));

    endMoveRows();
    return true;
}

void LayerListModel::refreshList()
{
    beginResetModel();
    endResetModel();
}

void LayerListModel::frameSelectionChanged(Frame* frame)
{
    setFrame(frame);
    refreshList();
}

