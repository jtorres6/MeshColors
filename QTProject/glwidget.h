/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "windows.h"
#include <GL/gl.h>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <iostream>
#include "vertex.h"
#include "colors.h"
#include "axis.h"
#include "tetrahedron.h"
#include "object3d_meshcolors.h"
#include "object3d_ply.h"
#include <QOpenGLBuffer>


namespace _gl_widget_ne {

  const float X_MIN=-.1;
  const float X_MAX=.1;
  const float Y_MIN=-.1;
  const float Y_MAX=.1;
  const float FRONT_PLANE_PERSPECTIVE=(X_MAX-X_MIN)/2;
  const float BACK_PLANE_PERSPECTIVE=1000;
  const float DEFAULT_DISTANCE=2;
  const float ANGLE_STEP=1;

  typedef enum {MODE_DRAW_POINT,MODE_DRAW_LINE,MODE_DRAW_FILL,MODE_DRAW_CHESS} _mode_draw;
  typedef enum {OBJECT_TETRAHEDRON,OBJECT_CUBE,OBJECT_MESHCOLORS} _object;
}

class _window;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _gl_widget : public QOpenGLWidget
{
Q_OBJECT
public:
    _gl_widget(_window *Window1);

    void clear_window();
    void change_projection();
    void change_observer();

    void draw_axis();
    void draw_objects();


protected:
    void resizeGL(int Width1, int Height1) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *Keyevent) Q_DECL_OVERRIDE;


private:

    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

    _window *Window;
    _axis Axis;
    _tetrahedron Tetrahedron;
    object3DPly ply;

    QOpenGLShaderProgram* program;
    QOpenGLVertexArrayObject *VAO, *VAO2;

    const char* p = "Models/ant.ply";
    object3DMeshColors *obj;

    _gl_widget_ne::_object Object;

    bool Draw_point;
    bool Draw_line;
    bool Draw_fill;
    bool Draw_chess;
    bool Draw_meshcolors;

    float Observer_angle_x = 0;
    float Observer_angle_y = 0;
    float Observer_distance = 1;
};

#endif