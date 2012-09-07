#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fimage.h"
#include "imgview.h"
#include "glview.h"

#include <QtGui/QMainWindow>
#include <QTabWidget>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void clickedOnImg(int x, int y);
    void disableGLView();

    void updateAll();
    void updateSG();
    void updateIMG();

    void openSlot();
    void saveSlot();

private:
    void loadImages(const QString &foreground, const QString &background);
    void setupUI();
    void loadSettings();
    void makeConnexions();

private:
    Ui::MainWindow *ui;
    QSettings _set;

    QTabWidget *_tabWidget;
    IMGView *_imgview;
    GLView *_glview;

    FImage _foreground;
    FImage _background;

    FImage _subtractionResult;
    FImage _sgFilterResult;
    QImage _image;
};

#endif // MAINWINDOW_H
