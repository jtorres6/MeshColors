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
  case Qt::Key_C:Draw_chess=!Draw_chess;break;
  case Qt::Key_M:Draw_meshcolors=!Draw_meshcolors;break;

  case Qt::Key_Left:Observer_angle_y-=ANGLE_STEP;
      qDebug() << Observer_angle_y;break;
  case Qt::Key_Right:Observer_angle_y+=ANGLE_STEP;break;
  case Qt::Key_Up:Observer_angle_x-=ANGLE_STEP;break;
  case Qt::Key_Down:Observer_angle_x+=ANGLE_STEP;
      qDebug() << Observer_angle_x;break;
  case Qt::Key_PageUp:Observer_distance*=1.2;
      qDebug() << Observer_distance; break;
  case Qt::Key_PageDown:Observer_distance/=1.2;
      qDebug() << Observer_distance;break;
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

    Projection.ortho(X_MIN, X_MAX, Y_MIN, Y_MAX, FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);

    /*Rotation_x.rotate(Observer_angle_x,1,0,0);
    Rotation_y.rotate(Observer_angle_y,0,1,0);
    Translation.translate(Observer_angle_x,Observer_angle_y,-Observer_distance);

    Projection*=Translation;
    Projection*=Rotation_x;
    Projection*=Rotation_y;

    Tetrahedron.Projection = Projection;
    Axis.Projection = Projection;*/
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->bind();
    VAO->bind();

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, 1);

    VAO->release();
    program->release();

  /*
    Axis.draw_line();

    Tetrahedron.Projection = Projection;
    obj->Projection = Projection;
    if(Draw_meshcolors)
    {
        obj->draw_texture();
    }
    else
    {
     if (Draw_point){
       glPointSize(5);
       glColor3fv((GLfloat *) &BLACK);
       switch (Object){
       case OBJECT_TETRAHEDRON:Tetrahedron.draw_point();break;
       case OBJECT_MESHCOLORS:obj->draw_point();break;
       default:break;
       }
     }

     if (Draw_line){
       glLineWidth(3);
       glColor3fv((GLfloat *) &MAGENTA);
       switch (Object){
       case OBJECT_TETRAHEDRON:Tetrahedron.draw_line();break;
       case OBJECT_MESHCOLORS:obj->draw_line();break;
       default:break;
       }
     }

     if (Draw_fill){
       glColor3fv((GLfloat *) &BLUE);
       switch (Object){
       case OBJECT_TETRAHEDRON:Tetrahedron.draw_fill();break;
       case OBJECT_MESHCOLORS:obj->draw_fill();break;
       default:break;
       }
     }

     if (Draw_chess){
       switch (Object){
       case OBJECT_TETRAHEDRON:Tetrahedron.draw_chess();break;
       default:break;
       }
     }
    }
    */

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

    program = new QOpenGLShaderProgram(context);
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"MeshColorsFragment.fsh");
    program->link();
    program->bind();

    VAO = new QOpenGLVertexArrayObject();
    VAO->create();
    VAO->bind();

    QOpenGLBuffer *positionBuffer = new QOpenGLBuffer();
    positionBuffer->create();
    positionBuffer->bind();
    positionBuffer->allocate(Axis.Vertices.data(), sizeof(Axis.Vertices.size() * 3 * sizeof(float)));

    QOpenGLBuffer *colorBuffer = new QOpenGLBuffer();
    colorBuffer->create();
    colorBuffer->bind();
    colorBuffer->allocate(Axis.Vertices.data(), sizeof(Axis.Vertices.size() * 3 * sizeof(float)));

    VAO->release();
    positionBuffer->release(
    colorBuffer->release();
    program->release();
    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);



    /*obj = new object3DMeshColors(p, context);
    obj->program = program;

    Tetrahedron.program = program;
    Axis.program = program;

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

    Observer_angle_x=0;
    Observer_angle_y=0;
    Observer_distance=DEFAULT_DISTANCE;

    Draw_point=false;
    Draw_line=true;
    Draw_fill=false;
    Draw_chess=false;*/
}
