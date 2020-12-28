#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLWindow>
#include <QSurfaceFormat>
#include <QOpenGLFunctions>
#include <QtOpenGL>
#include <GL/GLU.h>
#include <QMessageLogger>
#include <QFile>
#include "file_ply_stl.h"
#include <string>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QVector3D>
#include <stdlib.h>     /* srand, rand */
#include <QScreen>
#include <QOpenGLShaderProgram>

using namespace std;

class MainWindow : public QOpenGLWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    // Input
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void resizeEvent(QResizeEvent *event);
    void painEvent(QPaintEvent *event);

private:
    QOpenGLContext *context;
    QOpenGLFunctions *openGLFunctions;

    float rotationY;
    float rotationX;
    float distance = 3.0;

    QOpenGLTexture *textures[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    vector<float> Coordinates;
    vector<unsigned int> Positions;
    vector<QVector3D> VertexID;

    QImage image;

    double modelMatrix[16];
    double projMatrix[16];
    GLint viewport[4];

    QVector3D mousePosition;
    int pixel[3];

    int vertexLocation;
    int matrixLocation;
    int colorLocation;
    QOpenGLShaderProgram* program;

};
#endif // MAINWINDOW_H
