#ifndef GLVIEW_H
#define GLVIEW_H

#include "fimage.h"
#include <QGLWidget>

class GLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLView(QWidget *parent = 0);
    void setData(const FImage &data);

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    FImage _data;
};

#endif // GLVIEW_H
