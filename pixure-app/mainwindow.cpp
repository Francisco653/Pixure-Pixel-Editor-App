/**
 * Date:
 * Authors:
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "previewwindow.h"
#include "canvas.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    previewer = new PreviewWindow(&canvas, this);

    frameModel = new FrameListModel(&canvas, this);
    ui->frameList->setModel(frameModel);

    QModelIndex firstIndex = frameModel->index(0, 0);
    ui->frameList->setCurrentIndex(firstIndex);

    //Color Picker
    QColorDialog* colorPicker = new QColorDialog(this);
    QColor initialColor(255, 0, 0, 255);
    colorPicker->setCurrentColor(initialColor);
    colorPicker->setOption(QColorDialog::NoButtons);
    colorPicker->setWindowFlags(Qt::Widget);
    ui->colorPickerLayout->addWidget(colorPicker);
    colorPicker->setMinimumWidth(150);
    colorPicker->setMaximumWidth(220);
    colorPicker->setOption(QColorDialog::ShowAlphaChannel, true);

    connect(colorPicker, &QColorDialog::currentColorChanged, &canvas, &Canvas::setPenColor);

    //this stuff is styling the list of frames to appear nice
    ui->frameList->setViewMode(QListView::ListMode);
    ui->frameList->setFlow(QListView::LeftToRight);
    ui->frameList->setWrapping(false);
    ui->frameList->setMovement(QListView::Static);
    ui->frameList->setResizeMode(QListView::Fixed);

    ui->frameList->setDragEnabled(true);
    ui->frameList->setAcceptDrops(true);
    ui->frameList->setDropIndicatorShown(true);
    ui->frameList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->frameList->setDragDropOverwriteMode(false);
    ui->frameList->setDefaultDropAction(Qt::MoveAction);

    ui->frameList->setUniformItemSizes(true);
    ui->frameList->setSpacing(0);
    ui->frameList->setGridSize(QSize(100, 64));
    ui->frameList->setIconSize(QSize(64, 64));
    ui->frameList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->frameList->setStyleSheet(R"(
    QListView::item {
        margin: 2px;
        background: transparent;
        border: 3px solid #000000;
    }

    QListView::item:hover {
        background: #9bf29c;
        border: 3px solid #155716;
    }

    QListView::item:selected {
        background: transparent;
        border: 3px solid #0059ff;
    }

    QListView::item:selected:hover {
        background: transparent;
        border: 3px solid #0059ff;
    }
    )");

    ui->deleteFrameButton->setEnabled(canvas.canDelete());

    auto updateDeleteState = [&]() {
        ui->deleteFrameButton->setEnabled(canvas.canDelete());
    };

    // --- View to model ---
    // connects frame list clicking to the select frame slot
    connect(ui->frameList, &QListView::clicked, &canvas, &Canvas::selectFrame);

    //buttons connected from view to model
    connect(ui->addFrameButton, &QPushButton::clicked, this, [&]() {
        frameModel->appendFrame();
    });

    connect(ui->duplicateFrameButton, &QPushButton::clicked, this, [&]() {
        frameModel->duplicateFrame();
    });

    connect(ui->deleteFrameButton, &QPushButton::clicked, this, [&]() {
        frameModel->removeFrame();
    });

    // These are the drawing controls for our canvas widget, which will communicate to the canvas model.
    connect(ui->canvasWidget, &CanvasWidget::clickAtPixel, &canvas, &Canvas::pixelClicked);
    connect(ui->canvasWidget, &CanvasWidget::dragAcrossPixel, &canvas, &Canvas::pixelClicked);
    connect(ui->canvasWidget, &CanvasWidget::hoveredOverPixel, &canvas, &Canvas::pixelHovered);

    // keeps the selection highlighted on the frame when the selection updates
    connect(&canvas, &Canvas::frameSelected, this, [&](int index) {
        QModelIndex modelIndex = frameModel->index(index, 0);

        ui->frameList->setCurrentIndex(modelIndex);

        ui->frameList->selectionModel()->select(
            modelIndex,
            QItemSelectionModel::ClearAndSelect
            );
    });
    connect(&canvas, &Canvas::refreshFullCanvas, ui->canvasWidget, &CanvasWidget::onRefreshFullCanvas);

    // disables delete button if there is only 1 frame by connecting to signal that emits frame count
    connect(frameModel, &QAbstractItemModel::rowsInserted, this, updateDeleteState);
    connect(frameModel, &QAbstractItemModel::rowsRemoved, this, updateDeleteState);

    // updates the frame highlight in the view when rows are moved around (inserted, deleted, rearranged)
    connect(frameModel, &QAbstractItemModel::rowsMoved, this, [&]() {
        int idx = canvas.getActiveFrameIndex();
        QModelIndex modelIndex = frameModel->index(idx, 0);
        ui->frameList->setCurrentIndex(modelIndex);
    });
    connect(frameModel, &QAbstractItemModel::rowsInserted, this, [&]() {
        int idx = canvas.getActiveFrameIndex();
        QModelIndex modelIndex = frameModel->index(idx, 0);
        ui->frameList->setCurrentIndex(modelIndex);
    });

    connect(frameModel, &QAbstractItemModel::rowsRemoved, this, [&]() {
        int idx = canvas.getActiveFrameIndex();
        QModelIndex modelIndex = frameModel->index(idx, 0);
        ui->frameList->setCurrentIndex(modelIndex);
    });

    // 1. Handle the button toggle and text update via a Lambda
    connect(ui->onionSkinButton, &QPushButton::toggled, this, [this](bool checked) {
        // Update the UI text based on the toggle state
        ui->onionSkinButton->setText(checked ? "Onion Skin: On" : "Onion Skin: Off");

        // Pass the state directly to the Canvas model
        canvas.toggleOnionSkin(checked);
    });

    // Connects the Canvas model's onion skin signal to the Widget's receiver slot.
    // This allows the UI to display a faint "ghost" of the previous frame during animation/drawing.
    connect(&canvas, &Canvas::sendOnionSkin, ui->canvasWidget, &CanvasWidget::onReceiveOnionSkin);

    // CANVAS SIZE LOGIC
    ui->statusbar->addPermanentWidget(ui->canvasSizeDisplay);
    // Get dimensions from the model and show them in the UI
    ui->canvasSizeDisplay->setText(QString("%1 x %2").arg(canvas.getWidth()).arg(canvas.getHeight()));
    // Resizing Logic: Connect a signal to a Lambda to update the label
    // This allows the UI to change if the canvas ever resizes,
    // without needing a 'void updateLabel()' method.
    connect(&canvas, &Canvas::frameCountUpdated, this, [this](int count) {ui->canvasSizeDisplay->setText(QString("%1 x %2").arg(canvas.getWidth()).arg(canvas.getHeight()));});

    connect(ui->sizeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &text) {

        // Convert "32x32" string into the integer 32
        // We split by 'x' and take the first part
        int newSize = text.split('x').first().toInt();

        // Tell the Model (Canvas) to resize all frames and layers
        canvas.resizeCanvas(newSize);

        // Tell the View (CanvasWidget) to update its grid math
        ui->canvasWidget->initializeGrid(newSize, newSize);

        // Update the display with the new pixels
        // (This ensures the canvas doesn't look "blank" until the next mouse move)
        auto activeFrame = canvas.getFrame(canvas.getActiveFrameIndex());
        ui->canvasWidget->onRefreshFullCanvas(activeFrame->activeLayer->getPixels());
    });

    // MOUSE POSITION LOGIC
    ui->statusbar->addWidget(ui->mousePosDisplay);
    connect(ui->canvasWidget, &CanvasWidget::sendMousePosition, this, [this](int row, int col) {
        if ((row >= 0 && col >= 0) && (row < canvas.getHeight() && col < canvas.getWidth())) {
            ui->mousePosDisplay->setText(QString("Pixel: %1, %2").arg(col + 1).arg(row + 1));
        } else {
            ui->mousePosDisplay->setText("Pixel: -, -");
        }
    });

    //Here the Canvas model is updating the Canvas Widget View.
    connect(&canvas, &Canvas::erasePixelData, ui->canvasWidget, &CanvasWidget::erasedPixel);
    connect(&canvas, &Canvas::fillPixelArea, ui->canvasWidget, &CanvasWidget::filledPixelArea);
    connect(&canvas, &Canvas::colorShiftArea, ui->canvasWidget, &CanvasWidget::colorShiftedArea);
    connect(&canvas, &Canvas::updateSelection, ui->canvasWidget, &CanvasWidget::showSelection);
    connect(&canvas, &Canvas::updatePixelData, ui->canvasWidget, &CanvasWidget::updatedPixel);
    connect(&canvas, &Canvas::updatePixelData, frameModel, &FrameListModel::updateCurrentFrame);
    connect(&canvas, &Canvas::erasePixelData, frameModel, &FrameListModel::updateCurrentFrame);
    connect(&canvas, &Canvas::fillPixelArea, frameModel, &FrameListModel::updateCurrentFrame);
    connect(&canvas, &Canvas::colorShiftArea, frameModel, &FrameListModel::updateCurrentFrame);
    connect(&canvas, &Canvas::updateSelection, frameModel, &FrameListModel::updateCurrentFrame);

    //Save/Load
    connect(ui->saveButton, &QPushButton::clicked, this, [&]() {
        QString path = QFileDialog::getSaveFileName(this, "Save Sprite", "", "SSP (*.ssp)");
        if(!path.isEmpty())
            canvas.exportFile(path.toStdString());
    });
    connect(ui->loadButton, &QPushButton::clicked, this, [&]() {
        QString path = QFileDialog::getOpenFileName(this, "Load Sprite", "", "SSP (*.ssp)");
        if(!path.isEmpty()){
            canvas.importFile(path.toStdString());
            frameModel->refreshList(); //would need to rebuild after import
        }
    });
    //Tools
    connect(ui->drawButton, &QPushButton::clicked, &canvas, &Canvas::setPenModeDraw);
    connect(ui->eraseButton, &QPushButton::clicked, &canvas, &Canvas::setPenModeErase);
    connect(ui->fillButton, &QPushButton::clicked, &canvas, &Canvas::setPenModeFill);
    connect(ui->selectButton, &QPushButton::clicked, &canvas, &Canvas::setPenModeShapeSelect);
    connect(ui->colorShiftButton, &QPushButton::clicked, &canvas, &Canvas::setPenModeColorShift);


    //layer system

    // create the layer model using the currently active frame
    layerModel = new LayerListModel(canvas.getFrame(canvas.getActiveFrameIndex()), this);
    ui->layerList->setModel(layerModel);
    ui->layerList->selectionModel()->setCurrentIndex(layerModel->index(0, 0), QItemSelectionModel::ClearAndSelect);

    // styling the list of layers to match frame list
    ui->layerList->setViewMode(QListView::ListMode);
    ui->layerList->setWrapping(false);
    ui->layerList->setMovement(QListView::Static);
    ui->layerList->setResizeMode(QListView::Fixed);

    ui->layerList->setDragEnabled(true);
    ui->layerList->setAcceptDrops(true);
    ui->layerList->setDropIndicatorShown(true);
    ui->layerList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->layerList->setDragDropOverwriteMode(false);
    ui->layerList->setDefaultDropAction(Qt::MoveAction);

    ui->layerList->setUniformItemSizes(true);
    ui->layerList->setSpacing(6);
    ui->layerList->setGridSize(QSize(64, 100));
    ui->layerList->setIconSize(QSize(64, 64));
    ui->layerList->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->layerList->setStyleSheet(R"(
    QListView::item {
        margin: 2px;
        background: transparent;
        border: 3px solid #000000;
    }

    QListView::item:hover {
        background: #9bf29c;
        border: 3px solid #155716;
    }

    QListView::item:selected {
        background: transparent;
        border: 3px solid #0059ff;
    }

    QListView::item:selected:hover {
        background: transparent;
        border: 3px solid #0059ff;
    }
    )");

    // connects layer list clicking to the select layer function in the current frame
    connect(ui->layerList->selectionModel(), &QItemSelectionModel::currentChanged,
        this, [&](const QModelIndex& current, const QModelIndex&) {

        if (!current.isValid()) return;

        Frame* frame = canvas.getFrame(canvas.getActiveFrameIndex());
        if (!frame) return;

        frame->selectLayer(current);

        emit canvas.refreshFullCanvas(frame->getActiveLayerPixels());
    });

    // buttons connected from view to model (assumes buttons exist in ui)
    connect(ui->addLayerButton, &QPushButton::clicked, this, [&]() {
        layerModel->appendLayer();
    });

    connect(ui->deleteLayerButton, &QPushButton::clicked, this, [&]() {
        layerModel->removeLayer();

        Frame* frame = canvas.getFrame(canvas.getActiveFrameIndex());
        if (!frame) return;

        int idx = frame->getActiveLayerIndex();
        if (idx < 0 || idx >= frame->layerCount()) return;

        QModelIndex modelIndex = layerModel->index(idx, 0);
        if (!modelIndex.isValid()) return;

        ui->layerList->selectionModel()->setCurrentIndex(
            modelIndex,
            QItemSelectionModel::ClearAndSelect
            );
    });

    // when the active frame changes, updates which frame the layer model is pointing to
    connect(&canvas, &Canvas::frameSelected, this, [&](int index) {
        QModelIndex modelIndex = frameModel->index(index, 0);

        ui->frameList->setCurrentIndex(modelIndex);
        ui->frameList->selectionModel()->select(
            modelIndex,
            QItemSelectionModel::ClearAndSelect
        );

        Frame* frame = canvas.getFrame(index);
        layerModel->setFrame(frame);
        ui->layerList->update();

        // updates highlighted layer to match active layer in this frame
        int layerIdx = frame->getActiveLayerIndex();
        QModelIndex layerIndex = layerModel->index(layerIdx, 0);

        ui->layerList->selectionModel()->setCurrentIndex(
            layerIndex,
            QItemSelectionModel::ClearAndSelect
        );
    });

    connect(ui->playButton, &QPushButton::clicked, this, [this]() {
        if (!animationTimerActive) {
            // Read the FPS directly from the slider
            int currentFps = ui->fpsSlider->value();

            canvas.startAnimation(currentFps);

            previewer->show();

            ui->playButton->setText("Stop");
            animationTimerActive = true; // Actually toggle the state!
        } else {
            canvas.stopAnimation();
            ui->playButton->setText("Play");
            animationTimerActive = false; // Actually toggle the state!
        }
    });

    // Update the label and the live animation speed when the slider moves
    connect(ui->fpsSlider, &QSlider::valueChanged, this, [this](int newFps) {
        // Update the label to show the current number (if you added one)
        // ui->fpsLabel->setText(QString("FPS: %1").arg(newFps));

        // If the animation is currently running, update the canvas speed dynamically
        if (animationTimerActive) {
            canvas.setFPS(newFps);
        }
    });

    // This sends the data to the previewer window whenever the frame changes
    connect(&canvas, &Canvas::refreshFullCanvas, this, [this](const std::unordered_map<Point2D, QColor>& pixels) {
        if (previewer && previewer->isVisible()) {
            previewer->updatePreview(pixels);
        }
    });

    connect(&canvas, &Canvas::updatePixelData, layerModel, &LayerListModel::updateCurrentLayer);
    connect(&canvas, &Canvas::erasePixelData, layerModel, &LayerListModel::updateCurrentLayer);
    connect(&canvas, &Canvas::fillPixelArea, layerModel, &LayerListModel::updateCurrentLayer);
    connect(&canvas, &Canvas::colorShiftArea, layerModel, &LayerListModel::updateCurrentLayer);

    connect(&canvas, &Canvas::frameChanged, layerModel, &LayerListModel::frameSelectionChanged);

    // Change button colors to display current mode.
    QButtonGroup* toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);
    toolGroup->addButton(ui->drawButton);
    toolGroup->addButton(ui->eraseButton);
    toolGroup->addButton(ui->fillButton);
    toolGroup->addButton(ui->selectButton);
    toolGroup->addButton(ui->colorShiftButton);

    // Draw is the default tool
    ui->drawButton->setChecked(true);

    QString toolButtonStyle = R"(
    QPushButton {
        background: white;
        color: black;
    }
    QPushButton:checked {
        background: #ADD8E6;
        color: black;
    }
)";

    // apply style sheet
    ui->drawButton->setStyleSheet(toolButtonStyle);
    ui->eraseButton->setStyleSheet(toolButtonStyle);
    ui->fillButton->setStyleSheet(toolButtonStyle);
    ui->selectButton->setStyleSheet(toolButtonStyle);
    ui->colorShiftButton->setStyleSheet(toolButtonStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}
