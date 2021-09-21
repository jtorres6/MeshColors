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
#include <GL/gl.h>
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

    void pick(int Selection_position_x, int Selection_position_y);
    void DrawTrianglesSelectionMode();

    void MoveCameraRightLeft(QPair<qint32, qint32> InUnits);
    void AddCameraZoom(const float InValue);

    void SetObjectPath(const char* InNewPath);
    void SetMeshColorsArray(QVector<QVector4D> InColors);
    const QVector<QVector4D> GetMeshColorsArray() const;

    void SetCurrentPaintingColor(const QColor& InNewColor);

    void InitializeBuffer(QOpenGLShaderProgram* InShader, void* InData, const int InSize, const char* InName, const GLenum InType, const int InOffset, const int InStride);

public slots:
    void IncrementResolution();
    void DecreaseResolution();
    void EnableTriangleSelectionMode();
    void UpdatePencilSize(const int InValue);

protected:
    void resizeGL(int Width1, int Height1) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *Keyevent) Q_DECL_OVERRIDE;


    QOpenGLBuffer* GenerateBuffer(const void *data, int count);

    void UpdateSSBO(GLuint InSsbo, GLsizei InSize, void* InData);

    void LoadProgram();
    void CreateBuffers();
    void LogGlInfo();
public:
    _window *Window;

private:

    QOpenGLShaderProgram* program;
    QOpenGLShaderProgram* program2;
    QOpenGLVertexArrayObject *VAO, *VAO2, *VAO3, *VAO4;
    GLuint FBO;
    GLuint Color_texture;
    GLuint Depth_texture;

    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

    QMatrix4x4 Projection_light;
    QMatrix4x4 Rotation_x_light;
    QMatrix4x4 Rotation_y_light;
    QMatrix4x4 Translation_light;

    _axis Axis;
    _object3D object3d;

    const char* ModelFilePath = "../QTProject/Models/cube.ply";

    float Observer_angle_x = 0;
    float Observer_angle_y = 0;
    float Observer_distance = 1;

    float Light_angle_x = 0;
    float Light_angle_y = 0;
    float Light_distance = 1;

    bool TriangleSelectionMode = false;
    bool ColorLerpEnabled = false;

    bool LightingEnabled = true;
    bool DrawingSamplesID = false;

    int SelectedTriangle = -1;
    int PencilSize = 1;

    QVector3D points[1024];
    QOpenGLTexture *text;

    QOpenGLContext *context;
    GLuint ssbo;

    QColor CurrentPaintingColor = QColor(1.0f, 0.0f, 0.0f, 1.0f);

    QVector4D LightPosition = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
};

#endif
