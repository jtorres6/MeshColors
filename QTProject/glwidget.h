/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef GLWIDGET_H
#define GLWIDGET_H

#ifdef PLATFORM_WINDOWS
#include "windows.h"
#endif
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <iostream>
#include "vertex.h"
#include "colors.h"
#include "axis.h"
#include "object3d.h"
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <movableobject.h>


namespace _gl_widget_ne {

  const float X_MIN=-.1;
  const float X_MAX=.1;
  const float Y_MIN=-.1;
  const float Y_MAX=.1;
  const float FRONT_PLANE_PERSPECTIVE=(X_MAX-X_MIN)/2;
  const float BACK_PLANE_PERSPECTIVE=1000;
  const float DEFAULT_DISTANCE=2;
  const float ANGLE_STEP=0.5f;

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

    void pick(const int Selection_position_x, const int Selection_position_y);
    void drawTrianglesSelectionMode();

    void moveCameraRightLeft(QPair<qint32, qint32> InUnits);
    void addCameraZoom(const float InValue);

    void setObjectPath(const char* InNewPath);
    void setMeshColorsArray(QVector<QVector4D> InColors);
    void setResolutionsArray(QVector<int> InRes);

    const QVector<QVector4D>& getMeshColorsArray() const;
    const QVector<int>& getResolutionsArray() const;
    const _object3D *getObject3D() const;

    void setCurrentPaintingColor(const QColor& InNewColor);

    void initializeBuffer(QOpenGLShaderProgram* InShader, void* InData, const int InSize, const char* InName, const GLenum InType, const int InOffset, const int InStride);

public slots:
    void incrementResolution();
    void decreaseResolution();
    void enableTriangleSelectionMode();
    void setTriangleSelectionMode(bool active);

    void updatePencilSize(const int InValue);
    void updatePencilTransparency(const int InValue);
    void toggleLighting();
    void toggleColorInterpolation();
    void toggleWireframeMode();

protected:
    void resizeGL(int Width1, int Height1) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *Keyevent) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *Keyevent) Q_DECL_OVERRIDE;

    QOpenGLBuffer* generateBuffer(const void *data, int count);

    void updateSSBO(GLuint InSsbo, GLsizei InSize, void* InData);

    void loadProgram();
    void createBuffers();
    void logGlInfo();

public:
    _window *Window;

private:

    QOpenGLShaderProgram* program;
    QOpenGLShaderProgram* program2;

    QOpenGLVertexArrayObject *VAO;
    QOpenGLVertexArrayObject *VAO2;
    QOpenGLVertexArrayObject *VAO3;
    QOpenGLVertexArrayObject *VAO4;
    QOpenGLVertexArrayObject *VAO5;

    GLuint FBO;
    GLuint Color_texture;
    GLuint Depth_texture;

    MovableObject camera;
    MovableObject light;

    _axis Axis;
    _object3D object3d;

    const char* ModelFilePath = "";

    float Observer_angle_x = 0;
    float Observer_angle_y = 0;
    float Observer_pos_x = 0;
    float Observer_pos_y = 0;
    float Observer_distance = 1;

    float lightAngleX = 0;
    float lightAngleY = 0;
    float lightDistance = 1;

    bool TriangleSelectionMode = false;
    bool ColorLerpEnabled = false;
    bool wireframeMode = false;
    bool LightingEnabled = false;
    bool DrawingSamplesID = false;

    bool ControlPressed = false;
    bool ShiftPressed = false;

    int SelectedTriangleID = -1;
    QVector3D SelectedTriangle;
    QVector<QVector3D> SelectedTriangleDrawArray;

    int PencilSize = 10;
    float PencilTransparency = 0.5f;

    QVector3D points[1024];
    QOpenGLTexture *text;

    QOpenGLContext *context;
    GLuint ssbo;

    QColor CurrentPaintingColor = QColor(1.0f, 0.0f, 0.0f, 1.0f);

    QVector4D lightPosition = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
};

#endif
