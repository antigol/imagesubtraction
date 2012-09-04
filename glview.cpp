#include "glview.h"

GLView::GLView(QWidget *parent) :
    QGLWidget(parent)
{
}

void GLView::setData(const FImage &data)
{
    _data = data;
    updateGL();
}

void GLView::initializeGL()
{
}

void GLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLView::paintGL()
{
    qglClearColor(Qt::gray);
    glClear(GL_COLOR_BUFFER_BIT);
}
