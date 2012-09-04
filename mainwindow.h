#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fimage.h"
#include <QtGui/QMainWindow>
#include <QLabel>
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
    void updateAll();
    void updateSG();
    void updateIMG();

    void openSlot();
    void saveSlot();

private:
    QSettings _set;
    Ui::MainWindow *ui;
    QLabel *_label;

    FImage _foreground;
    FImage _background;

    FImage _subtractionResult;
    FImage _sgFilterResult;
    QImage _image;
};

#endif // MAINWINDOW_H
