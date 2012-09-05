#include "glview.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <cmath>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

GLView::GLView(QWidget *parent) :
    QGLWidget(parent),
    _vbo(QGLBuffer::VertexBuffer), _ibo(QGLBuffer::IndexBuffer),
    _set("tp-ph-epfl", "diffimages")
{
    _min = 0.0;
    _max = 1.0;
    _zScale = 1.0;

    _alpha = _set.value("alpha", 0.0).toDouble();
    _beta = _set.value("beta", 0.0).toDouble();
    _zoom = _set.value("zoom", 1.0).toDouble();
}

GLView::~GLView()
{
    _set.setValue("alpha", _alpha);
    _set.setValue("beta", _beta);
    _set.setValue("zoom", _zoom);
}

void GLView::setData(const FImage &data)
{
    _needUpdateVBO = true;
    _data = data;
    updateGL();
}

void GLView::setMinMax(float min, float max)
{
    _min = min;
    _max = max;
}

void GLView::setZScale(float zScale)
{
    _zScale = zScale;
}

void GLView::initializeGL()
{
    if (QGLShaderProgram::hasOpenGLShaderPrograms()) {
        _render3D = new QGLShaderProgram(this);
        _render3D->addShaderFromSourceFile(QGLShader::Vertex, ":/render3D.vert");
        _render3D->addShaderFromSourceFile(QGLShader::Fragment, ":/render3D.frag");
        _render3D->bindAttributeLocation("zvalue", 0);
        _render3D->bindAttributeLocation("vertex", 1);
        _render3D->link();
    } else {
        QMessageBox::critical(this, "OpenGL Shader", "OpenGL Shader are not supported !");
        _render3D = 0;
    }
}

void GLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLView::paintGL()
{
    if (_render3D == 0) {
        qglClearColor(Qt::red);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if (_needUpdateVBO) {
        _needUpdateVBO = false;
        fillVBO();
    }

    QMatrix4x4 m;
    float aspect = float(width()) / float(height());
    m.ortho(-_zoom * aspect, _zoom * aspect, -_zoom, _zoom, 0.0001, 1000.0);
    m.translate(0.0, 0.0, -2);
    m.rotate(_beta, 1.0, 0.0, 0.0);
    m.rotate(_alpha, 0.0, 0.0, 1.0);
    m.translate(0.0, 0.0, -(_min + _max) / 2.0);

    qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _render3D->bind();
    _render3D->setUniformValue("cmend", _max);
    _render3D->setUniformValue("cmbeg", _min);
    _render3D->setUniformValue("zscale", _zScale);
    _render3D->setUniformValue("matrix", m);
    _render3D->setUniformValue("aspect", GLfloat(_data.width) / GLfloat(_data.height));

    _vbo.bind();
    _render3D->setAttributeBuffer(0, GL_FLOAT, 0, 1); // zvalue
    _render3D->setAttributeBuffer(1, GL_FLOAT, _data.width*_data.height*sizeof (GLfloat), 2); // vertex
    _vbo.release();

    _render3D->enableAttributeArray(0);
    _render3D->enableAttributeArray(1);

    _ibo.bind();
    for (int y = 0; y < _data.height-1; ++y) {
        glDrawElements(GL_TRIANGLE_STRIP, 2*_data.width, GL_UNSIGNED_INT, BUFFER_OFFSET(y * 2*_data.width * sizeof (GLuint)));
    }
    _ibo.release();

    _render3D->disableAttributeArray(0);
    _render3D->disableAttributeArray(1);

    _render3D->release();
}

void GLView::fillVBO()
{
    if (!_vbo.isCreated())
        _vbo.create();
    if (!_ibo.isCreated())
        _ibo.create();

    QVector<GLfloat> vertices;
    for (GLfloat y = 0; y < _data.height; ++y) {
        for (GLfloat x = 0; x < _data.width; ++x) {
            vertices << (x) / GLfloat(_data.width)
                     << (y) / GLfloat(_data.height);
        }
    }

    _vbo.bind();
    _vbo.allocate((_data.data.size() + vertices.size()) * sizeof (GLfloat));
    _vbo.write(0, _data.data.constData(), _data.data.size() * sizeof (GLfloat));
    _vbo.write(_data.data.size() * sizeof (GLfloat), vertices.constData(), vertices.size() * sizeof (GLfloat));
    _vbo.release();

    QVector<GLuint> indices;
    indices.reserve(2 * _data.width * (_data.height-1));
    for (int y = 1; y < _data.height; ++y) {
        for (int x = 0; x < _data.width; ++x) {
            indices << y * _data.width + x;
            indices << (y-1) * _data.width + x;
        }
    }
    _ibo.bind();
    _ibo.allocate(indices.constData(), indices.size() * sizeof (GLuint));
    _ibo.release();
}

void GLView::mousePressEvent(QMouseEvent *e)
{
    _lastMousePosition = e->posF();
}

void GLView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF d = e->posF() - _lastMousePosition;
    _lastMousePosition = e->posF();

    _alpha += d.x();
    _beta += d.y();
    if (_beta < -90.0)
        _beta = -90.0;
    if (_beta > 0.0)
        _beta = 0.0;
    updateGL();
}

void GLView::wheelEvent(QWheelEvent *e)
{
    _zoom *= std::pow(0.9995, e->delta());
    if (_zoom < 0.01)
        _zoom = 0.01;
    updateGL();
}

void GLView::timerEvent(QTimerEvent *)
{
    updateGL();
}
