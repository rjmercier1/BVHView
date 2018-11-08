#include <iostream>

#include <QtGui>
#include <QGLWidget>
#include <QOpenGLShaderProgram>

#include <OpenGL/GLU.h>
#include "myqglwidget.h"


MyQGLWidget::MyQGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    camera_yaw = 0.0;
    camera_pitch = -20.0;
    cam_distance = 10;
    bvh = NULL;
    frame_no = 1;
    playing = true;
    startTimer(42);     // 24 fps
}

void
MyQGLWidget::loadFile(QString s)
{
    if (bvh)
        delete bvh;
    bvh = new BVH(s.toStdString());
    update();
}

void
MyQGLWidget::initializeGL()
{
    GLenum err;

    initializeOpenGLFunctions();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0.55, 0.55, 0.55, 1.0);

}

void MyQGLWidget::drawGrid()
{
    float  size = 1.5f;
    int  num_x = 10, num_z = 10;
    double  ox, oz;
    glBegin( GL_QUADS );
        glNormal3d( 0.0, 1.0, 0.0 );
        ox = -(num_x * size) / 2;
        for ( int x=0; x<num_x; x++, ox+=size )
        {
            oz = -(num_z * size) / 2;
            for ( int z=0; z<num_z; z++, oz+=size )
            {
                if ( ((x + z) % 2) == 0 )
                    glColor3f( 1.0, 1.0, 1.0 );
                else
                    glColor3f( 0.8, 0.8, 0.8 );
                glVertex3d( ox, 0.0, oz );
                glVertex3d( ox, 0.0, oz+size );
                glVertex3d( ox+size, 0.0, oz+size );
                glVertex3d( ox+size, 0.0, oz );
            }
        }
    glEnd();
}

void
MyQGLWidget::paintGL()
{
    GLenum err;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -cam_distance);
    glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
    glRotatef(-camera_yaw,   0.0, 1.0, 0.0);
    glTranslatef(0, -0.5, 0);
    drawGrid();
    if (bvh)
        bvh->RenderFigure(frame_no, 1.0);
}

void
MyQGLWidget::resizeGL(int w, int h)
{
    float aspect = (float)w/(float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, aspect, 0.1, 50);
    win_width = w;
    win_height = h;
}

void
MyQGLWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->modifiers() & Qt::ControlModifier) {
    } else if (ev->modifiers() & Qt::AltModifier) {
    } else if (ev->modifiers() & Qt::MetaModifier) {
    }
    last_mouse_x = ev->x();
    last_mouse_y = ev->y();
}

void
MyQGLWidget::mouseMoveEvent(QMouseEvent *ev)
{


    if (ev->modifiers() & Qt::ControlModifier) {
        cam_distance += ( ev->y() - last_mouse_y ) * 0.2;
        if ( cam_distance < 2.0 )
            cam_distance = 2.0;
    } else {
        camera_yaw -= (ev->x() - last_mouse_x) * 1.0;
        if (camera_yaw < 0)
            camera_yaw += 360.0;
        else if (camera_yaw > 360.0)
            camera_yaw -= 360;

        camera_pitch -= (ev->y() - last_mouse_y) * 1.0;
        if (camera_pitch < -90)
            camera_pitch = -90;
        else if (camera_pitch > 90.0)
            camera_pitch = 90.0;
    }

    last_mouse_x = ev->x();
    last_mouse_y = ev->y();
    update();
}

void
MyQGLWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    update();
}

void
MyQGLWidget::keyPressEvent(QKeyEvent *ev)
{
    printf("%c\n", ev->key());
    switch (ev->key()) {
    case Qt::Key_H:
        update();
        break;
    case Qt::Key_Space:
        playing = !playing;
        break;
    case Qt::Key_Left:
        frame_no--;
        if (frame_no < 1)
            frame_no = bvh->GetNumFrame() - 1;
        update();
        break;
    case Qt::Key_Right:
        frame_no++;
        if (frame_no >= bvh->GetNumFrame())
            frame_no = 1;
        update();
        break;
    }
}

void
MyQGLWidget::timerEvent(QTimerEvent *event)
{
    if (bvh) {
        if (playing) {
            frame_no = 1 + (frame_no + 1) % (bvh->GetNumFrame() - 1);
            update();
        }
    }
}
