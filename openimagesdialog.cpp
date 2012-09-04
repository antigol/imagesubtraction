#include "openimagesdialog.h"
#include "ui_openimagesdialog.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>

OpenImagesDialog::OpenImagesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenImagesDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));

    _process = new QProcess(this);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedProcess(int,QProcess::ExitStatus)));

    connect(ui->foregroundLineEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyFilepath()));
    connect(ui->backgroundLineEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyFilepath()));

    ui->foregroundLineEdit->setText(_set.value("foregroundfilepath").toString());
    ui->backgroundLineEdit->setText(_set.value("backgroundfilepath").toString());
}

OpenImagesDialog::~OpenImagesDialog()
{
    delete ui;
}

QString OpenImagesDialog::foregroundFilepath() const
{
    return ui->foregroundLineEdit->text();
}

QString OpenImagesDialog::backgroundFilepath() const
{
    return ui->backgroundLineEdit->text();
}

void OpenImagesDialog::verifyFilepath()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (lineEdit) {
        QPalette palette = lineEdit->palette();
        if (QFileInfo(lineEdit->text()).exists()) {
            palette = QPalette();
        } else {
            palette.setColor(QPalette::Base, Qt::red);
        }
        lineEdit->setPalette(palette);
    }
}

void OpenImagesDialog::on_foregroundToolButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "foreground", ui->foregroundLineEdit->text());
    if (!file.isEmpty()) {
        ui->foregroundLineEdit->setText(file);
    }
}

void OpenImagesDialog::on_backgroundToolButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "background", ui->backgroundLineEdit->text());
    if (!file.isEmpty()) {
        ui->backgroundLineEdit->setText(file);
    }
}

void OpenImagesDialog::on_alignPushButton_clicked()
{
    QString prefix = QInputDialog::getText(this, "prefix", "choose a prefix : <prefix>0000.tif", QLineEdit::Normal, "aligned");
    if (!prefix.isEmpty()) {
        QFileInfo info(ui->foregroundLineEdit->text());
        _outputFilePath = QFileInfo(info.path(), prefix).filePath();
        _process->setWorkingDirectory(info.path());
        _process->start("align_image_stack", QStringList() << "-a" << prefix << ui->foregroundLineEdit->text() << ui->backgroundLineEdit->text());

        ui->alignLabel->setText("Running...");
        ui->alignPushButton->setEnabled(false);
    }
}

void OpenImagesDialog::finishedProcess(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode == 0 && status == QProcess::NormalExit) {
        ui->foregroundLineEdit->setText(_outputFilePath + "0000.tif");
        ui->backgroundLineEdit->setText(_outputFilePath + "0001.tif");

        ui->alignLabel->setText("Success !");
        ui->alignPushButton->setEnabled(true);
    } else {
        ui->alignLabel->setText("Connot run align_image_stack. Maybe he is not installed");
    }
}

void OpenImagesDialog::saveSettings()
{
    _set.setValue("foregroundfilepath", ui->foregroundLineEdit->text());
    _set.setValue("backgroundfilepath", ui->backgroundLineEdit->text());
}
