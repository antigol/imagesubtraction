#include "mainwindow.h"

#include "openimagesdialog.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QSplitter>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _set("tp-ph-epfl", "diffimages")
{
    setupUI();
    loadSettings();
    makeConnexions();

    if (_set.contains("foregroundfilepath") && _set.contains("backgroundfilepath")) {
        loadImages(_set.value("foregroundfilepath").toString(), _set.value("backgroundfilepath").toString());
        updateAll();
    }
}

MainWindow::~MainWindow()
{
    _set.setValue("radio1", ui->radioButton->isChecked());
    _set.setValue("bf", ui->bfDoubleSpinBox->value());
    _set.setValue("sg", ui->sgFilterCheckBox->isChecked());
    _set.setValue("min", ui->minValueDoubleSpinBox->value());
    _set.setValue("max", ui->maxValueDoubleSpinBox->value());
    _set.setValue("zscale", ui->zScaleDoubleSpinBox->value());
}

void MainWindow::clickedOnImg(int x, int y)
{
    ui->bfDoubleSpinBox->setValue(_foreground.pixel(x, y) / _background.pixel(x, y));
}

void MainWindow::disableGLView()
{
    _tabWidget->setTabEnabled(1, false);
}

void MainWindow::updateAll()
{
    if (_foreground.width * _foreground.height * _background.width * _background.height == 0) {
        statusBar()->showMessage("foreground or background null");
        return;
    }
    if (_foreground.width != _background.width || _foreground.height != _background.height) {
        statusBar()->showMessage("foreground and background haven't the same size");
        return;
    }

    if (ui->radioButton->isChecked()) {
        FImage::subtract(_foreground, _background, ui->bfDoubleSpinBox->value(), _subtractionResult);
    } else {
        FImage::subtract(_background, ui->bfDoubleSpinBox->value(), _foreground, _subtractionResult);
    }

    updateSG();
}

void MainWindow::updateSG()
{
    if (ui->sgFilterCheckBox->isChecked())
        FImage::sgfilter(_subtractionResult, _sgFilterResult);
    else
        _sgFilterResult = _subtractionResult;

    updateIMG();
}

void MainWindow::updateIMG()
{
    if (ui->minValueDoubleSpinBox->value() >= ui->maxValueDoubleSpinBox->value()) {
        statusBar()->showMessage("min >= max");
        return;
    }

    _image = _sgFilterResult.render(ui->minValueDoubleSpinBox->value(), ui->maxValueDoubleSpinBox->value());
    _imgview->setPixmap(QPixmap::fromImage(_image));

    _glview->setMinMax(ui->minValueDoubleSpinBox->value(), ui->maxValueDoubleSpinBox->value());
    _glview->setZScale(ui->zScaleDoubleSpinBox->value());
    _glview->setData(_sgFilterResult);

    statusBar()->showMessage("image ok", 500);
}

void MainWindow::openSlot()
{
    OpenImagesDialog dialog;
    if (dialog.exec()) {
        loadImages(dialog.foregroundFilepath(), dialog.backgroundFilepath());
        updateAll();
    }
}

void MainWindow::saveSlot()
{
    QString filepath = QFileDialog::getSaveFileName(this, "capture filepath", _set.value("capture").toString());
    if (!filepath.isEmpty()) {
        _set.setValue("capture", filepath);
        if (_tabWidget->currentIndex() == 0)
            _image.save(filepath);
        else
            _glview->grabFrameBuffer().save(filepath);
    }
}

void MainWindow::loadImages(const QString &foreground, const QString &background)
{
    QImage image;
    if (!image.load(foreground))
        QMessageBox::critical(this, "QImage::load", "Cannot load foreground : " + foreground);
    _foreground = FImage(image);
    ui->label_foreground->setPixmap(QPixmap::fromImage(image.scaledToWidth(150)));

    if (!image.load(background))
        QMessageBox::critical(this, "QImage::load", "Cannot load background : " + background);
    _background = FImage(image);
    ui->label_background->setPixmap(QPixmap::fromImage(image.scaledToWidth(150)));
}

void MainWindow::setupUI()
{
    ui->setupUi(this);

    _imgview = new IMGView(this);
    _imgview->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(_imgview);

    _glview = new GLView(this);

    _tabWidget = new QTabWidget(this);
    _tabWidget->insertTab(0, scrollArea, "flat");
    _tabWidget->insertTab(1, _glview, "relief");
    setCentralWidget(_tabWidget);

}

void MainWindow::loadSettings()
{
    ui->radioButton->setChecked(_set.value("radio1", true).toBool());
    ui->radioButton_2->setChecked(!_set.value("radio1", true).toBool());
    ui->bfDoubleSpinBox->setValue(_set.value("bf", 1.0).toDouble());
    ui->sgFilterCheckBox->setChecked(_set.value("sg", false).toBool());
    ui->minValueDoubleSpinBox->setValue(_set.value("min", 0.0).toDouble());
    ui->maxValueDoubleSpinBox->setValue(_set.value("max", 1.0).toDouble());
    ui->zScaleDoubleSpinBox->setValue(_set.value("zscale", 0.4).toDouble());
}

void MainWindow::makeConnexions()
{
    connect(ui->actionOpen_images, SIGNAL(triggered()), this, SLOT(openSlot()));
    connect(ui->actionCapture, SIGNAL(triggered()), this, SLOT(saveSlot()));

    connect(_imgview, SIGNAL(clickOnPixmap(int,int)), this, SLOT(clickedOnImg(int,int)));
    connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(updateAll()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(updateAll()));
    connect(ui->bfDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateAll()));
    connect(ui->sgFilterCheckBox, SIGNAL(clicked()), this, SLOT(updateSG()));
    connect(ui->minValueDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateIMG()));
    connect(ui->maxValueDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateIMG()));
    connect(ui->zScaleDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateIMG()));

    connect(_glview, SIGNAL(shaderUnsupported()), this, SLOT(disableGLView()));
}
