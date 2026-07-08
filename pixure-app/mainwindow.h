/**
 * Date:
 * Authors:
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include "canvas.h"
#include "framelistmodel.h"
#include "layerlistmodel.h"
#include "canvaswidget.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QButtonGroup>

class PreviewWindow;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Canvas canvas = Canvas(16, 16);
    PreviewWindow* previewer;
    FrameListModel* frameModel;
    LayerListModel* layerModel;;

    bool animationTimerActive = false;
};
#endif // MAINWINDOW_H
