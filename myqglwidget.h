#ifndef MYQGLWIDGET_H
#define MYQGLWIDGET_H

#include <ImathVec.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "bvh.h"

using namespace Imath;

class MyQGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    MyQGLWidget(QWidget *parent = 0);

    void loadFile(QString s);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *ev);
    void timerEvent(QTimerEvent *event);
    void drawGrid();
    void drawAxes();

private:
    int win_width, win_height;
    float  camera_yaw, camera_pitch, cam_distance;
    int last_mouse_x, last_mouse_y;
    bool playing;

    BVH *bvh;
    int frame_no;

};

#endif // MYQGLWIDGET_H
