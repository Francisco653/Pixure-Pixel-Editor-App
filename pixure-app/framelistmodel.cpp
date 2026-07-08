/**
 * Date:
 * Authors:
 */

#include "framelistmodel.h"

FrameListModel::FrameListModel(Canvas *canvas, QObject* parent)
    : QAbstractListModel(parent)
    , canvas(canvas) {}

int FrameListModel::rowCount(const QModelIndex&) const {
    return canvas->frameCount();
}

void FrameListModel::appendFrame()
{
    int idx = canvas->frameCount();
    beginInsertRows(QModelIndex(), idx, idx);

    canvas->addFrame();

    endInsertRows();
}

void FrameListModel::removeFrame()
{
    int index = canvas->getActiveFrameIndex();
    beginRemoveRows(QModelIndex(), index, index);

    canvas->deleteFrame();

    endRemoveRows();
}

void FrameListModel::duplicateFrame()
{
    int idx = canvas->getActiveFrameIndex();
    beginInsertRows(QModelIndex(), idx + 1, idx + 1);

    canvas->duplicateFrame();

    endInsertRows();
}

QVariant FrameListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    int itemIndex = index.row();
    Frame* frame = canvas->getFrame(itemIndex);
    if (!frame)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        // return QString("Frame %1").arg(itemIndex);
    }

    if (role == Qt::DecorationRole)
    {
        Frame* frame = canvas->getFrame(itemIndex);

        auto pixels = frame->getPixels();

        int w = canvas->getWidth();
        int h = canvas->getHeight();

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

Qt::DropActions FrameListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void FrameListModel::refreshList()
{
    beginResetModel();
    endResetModel();
}

void FrameListModel::updateCurrentFrame()
{
    int row = canvas->getActiveFrameIndex();
    QModelIndex idx = index(row, 0);
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}

Qt::ItemFlags FrameListModel::flags(const QModelIndex &index) const
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

bool FrameListModel::moveRows(const QModelIndex &, int originalIndex, int count, const QModelIndex &, int destinationIndex)
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

    canvas->reorderFrame(itemIndex, originalIndex);
    canvas->selectFrame(index(itemIndex, 0));

    endMoveRows();
    return true;
}
