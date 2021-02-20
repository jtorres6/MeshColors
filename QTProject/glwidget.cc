/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "glwidget.h"
#include "window.h"

using namespace std;
using namespace _gl_widget_ne;
using namespace _colors_ne;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_gl_widget::_gl_widget(_window *Window1):Window(Window1)
{
  setMinimumSize(300, 300);
  setFocusPolicy(Qt::StrongFocus);
}


/*****************************************************************************//**
 * Evento tecla pulsada
 *
 *
 *
 *****************************************************************************/

void _gl_widget::keyPressEvent(QKeyEvent *Keyevent)
{
  switch(Keyevent->key()){
  case Qt::Key_1:Object=OBJECT_TETRAHEDRON;break;
  case Qt::Key_2:Object=OBJECT_CUBE;break;
  case Qt::Key_3:Object=OBJECT_MESHCOLORS;break;

  case Qt::Key_P:Draw_point=!Draw_point;break;
  case Qt::Key_L:Draw_line=!Draw_line;break;
  case Qt::Key_F:Draw_fill=!Draw_fill;break;

  case Qt::Key_Left:Observer_angle_y-=ANGLE_STEP;break;
  case Qt::Key_Right:Observer_angle_y+=ANGLE_STEP;break;
  case Qt::Key_Up:Observer_angle_x-=ANGLE_STEP;break;
  case Qt::Key_Down:Observer_angle_x+=ANGLE_STEP;break;
  case Qt::Key_PageUp:Observer_distance*=1.2;break;
  case Qt::Key_PageDown:Observer_distance/=1.2;break;
  }

  update();
}


/*****************************************************************************//**
 * Limpiar ventana
 *
 *
 *
 *****************************************************************************/

void _gl_widget::clear_window()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}



/*****************************************************************************//**
 * Funcion para definir la transformación de proyeccion
 *
 *
 *
 *****************************************************************************/

void _gl_widget::change_projection()
{

}



/*****************************************************************************//**
 * Funcion para definir la transformación de vista (posicionar la camara)
 *
 *
 *
 *****************************************************************************/

void _gl_widget::change_observer()
{



}


/*****************************************************************************//**
 * Funcion que dibuja los objetos
 *
 *
 *
 *****************************************************************************/

void _gl_widget::draw_objects()
{
    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

    Projection.frustum(X_MIN, X_MAX, Y_MIN, Y_MAX, FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);
    Rotation_x.rotate(Observer_angle_x,1,0,0);
    Rotation_y.rotate(Observer_angle_y,0,1,0);
    Translation.translate(0,0,-Observer_distance);

    Projection*=Translation;
    Projection*=Rotation_x;
    Projection*=Rotation_y;

    program->bind();
    VAO->bind();

    int matrixLocation = program->uniformLocation("matrix");
    program->setUniformValue(matrixLocation, Projection);

    glDrawArrays(GL_LINES, 0, Axis.Vertices.size());

    if(glGetError() != GL_NO_ERROR)
    {

        qDebug() << "GL ERROR";
    }

    VAO->release();

    VAO2->bind();

    int matrixLocation2 = program->uniformLocation("matrix");
    program->setUniformValue(matrixLocation2, Projection);

    if(Draw_point)
    {
        glPointSize(10);
        glDrawArrays(GL_POINTS, 0, ply.VerticesDrawArrays.size());
    }
    if(Draw_line)
    {
        glDrawArrays(GL_LINES, 0, ply.VerticesDrawArrays.size());
    }

    if(Draw_fill)
    {
        glDrawArrays(GL_TRIANGLES,0, ply.VerticesDrawArrays.size());
    }

    if(glGetError() != GL_NO_ERROR)
    {
        qDebug() << "GL ERROR";
    }

    VAO2->release();
    program->release();
}



/*****************************************************************************//**
 * Evento de dibujado
 *
 *
 *
 *****************************************************************************/

void _gl_widget::paintGL()
{
  clear_window();
  change_projection();
  change_observer();
  draw_objects();
}


/*****************************************************************************//**
 * Evento de cambio de tamaño de la ventana
 *
 *
 *
 *****************************************************************************/

void _gl_widget::resizeGL(int Width1, int Height1)
{
  glViewport(0,0,Width1,Height1);
}


/*****************************************************************************//**
 * Inicialización de OpenGL
 *
 *
 *
 *****************************************************************************/

void _gl_widget::initializeGL()
{
    const GLubyte* strm;
    QOpenGLContext *context;
    context = new QOpenGLContext(this);
    Axis = _axis(500.0f);
    Tetrahedron = _tetrahedron(0.5f);

    ply = object3DPly(p);

    program = new QOpenGLShaderProgram(context);
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"MeshColorsFragment.fsh");
    program->link();
    program->bind();

     // VAO 1
    VAO = new QOpenGLVertexArrayObject();
    VAO->create();
    VAO->bind();

    QOpenGLBuffer *positionBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer->create();
    positionBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer->bind();
    positionBuffer->allocate(Axis.Vertices.data(), Axis.Vertices.size() * sizeof(QVector3D));
    positionBuffer->release();

    QOpenGLBuffer *colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colorBuffer->create();
    colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    colorBuffer->bind();
    colorBuffer->allocate(Axis.Colors.data(), Axis.Colors.size() * sizeof(QVector3D));
    colorBuffer->release();

    positionBuffer->bind();
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer->bind();
    program->enableAttributeArray("color");
    program->setAttributeBuffer("color", GL_FLOAT, 0, 3);
    colorBuffer->release();

    VAO->release();

    // VAO 2
    VAO2 = new QOpenGLVertexArrayObject();
    VAO2->create();
    VAO2->bind();

    positionBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer->create();
    positionBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer->bind();
    positionBuffer->allocate(ply.VerticesDrawArrays.data(), ply.VerticesDrawArrays.size() * sizeof(QVector3D));
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colorBuffer->create();
    colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    colorBuffer->bind();
    colorBuffer->allocate(ply.colors.data(), ply.colors.size() * sizeof(QVector4D));
    program->enableAttributeArray("color");
    program->setAttributeBuffer("color", GL_FLOAT, 0, 3);

    VAO2->release();

    program->release();

    strm = glGetString(GL_VENDOR);
    std::cerr << "Vendor: " << strm << "\n";
    strm = glGetString(GL_RENDERER);
    std::cerr << "Renderer: " << strm << "\n";
    strm = glGetString(GL_VERSION);
    std::cerr << "OpenGL Version: " << strm << "\n";

    if (strm[0] == '1'){
    std::cerr << "Only OpenGL 1.X supported!\n";
    exit(-1);
    }

    strm = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cerr << "GLSL Version: " << strm << "\n";

    int Max_texture_size=0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Max_texture_size);
    std::cerr << "Max texture size: " << Max_texture_size << "\n";

    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);


    Observer_angle_x=0;
    Observer_angle_y=0;
    Observer_distance=DEFAULT_DISTANCE;

    Draw_point=false;
    Draw_line=true;
    Draw_fill=false;
}
