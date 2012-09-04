#-------------------------------------------------
#
# Project created by QtCreator 2012-09-04T10:19:14
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = imagesubtraction
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fimage.cpp \
    glview.cpp \
    openimagesdialog.cpp

HEADERS  += mainwindow.h \
    fimage.h \
    glview.h \
    openimagesdialog.h

FORMS += \
    openimagesdialog.ui \
    mainwindow.ui
