#-------------------------------------------------
#
# Project created by QtCreator 2012-09-04T10:19:14
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = diffimages
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fimage.cpp \
    glview.cpp \
    openimagesdialog.cpp \
    imgview.cpp

HEADERS  += mainwindow.h \
    fimage.h \
    glview.h \
    openimagesdialog.h \
    imgview.h

FORMS += \
    openimagesdialog.ui \
    mainwindow.ui

RC_FILE = icon.rc
ICON = icon.xpm

RESOURCES += \
    files.qrc

OTHER_FILES += \
    render3D.vert \
    render3D.frag
