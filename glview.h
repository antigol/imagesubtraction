#ifndef GLVIEW_H
#define GLVIEW_H

#include "fimage.h"
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QSettings>

class GLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLView(QWidget *parent = 0);
    ~GLView();

    void setData(const FImage &data);
    void setMinMax(float min, float max);
    void setZScale(float zScale);

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void fillVBO();

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void timerEvent(QTimerEvent *);

    FImage _data;

    bool _needUpdateVBO;
    QGLShaderProgram *_render3D;
    QGLBuffer _vbo, _ibo;

    double _zoom;
    double _alpha;
    double _beta;

    GLfloat _min;
    GLfloat _max;

    GLfloat _zScale;

    QPointF _lastMousePosition;
    QSettings _set;
};

#endif // GLVIEW_H
