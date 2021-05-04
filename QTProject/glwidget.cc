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
    Rotation_x.rotate(Observer_angle_x, 1, 0, 0);
    Rotation_y.rotate(Observer_angle_y, 0, 1, 0);
    Translation.translate(0, 0, -Observer_distance);

    Projection*=Translation;
    Projection*=Rotation_x;
    Projection*=Rotation_y;

    program->bind();

    // Draw axis
    VAO->bind();

    int matrixLocation = program->uniformLocation("matrix");
    program->setUniformValue(matrixLocation, Projection);

    glDrawArrays(GL_LINES, 0, Axis.Vertices.size());

    VAO->release();

    // Draw 3D model
    VAO2->bind();

    int matrixLocation2 = program->uniformLocation("matrix");
    program->setUniformValue(matrixLocation2, Projection);

    if(Draw_point)
    {
        glPointSize(10);
        glDrawArrays(GL_POINTS, 0, object3d.VerticesDrawArrays.size());
    }
    if(Draw_line)
    {
        glDrawArrays(GL_LINES, 0, object3d.VerticesDrawArrays.size());
    }
    if(Draw_fill)
    {
        glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());
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

    object3d = _object3D(p);

    program = new QOpenGLShaderProgram(context);
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "MeshColorsGeometry.gsh");
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
    positionBuffer->allocate(object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D));
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colorBuffer->create();
    colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    colorBuffer->bind();
    colorBuffer->allocate(object3d.Colors.data(), object3d.Colors.size() * sizeof(QVector4D));
    program->enableAttributeArray("color");
    program->setAttributeBuffer("color", GL_FLOAT, 0, 4);
    colorBuffer->release();

    QOpenGLBuffer *VertexIndexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    VertexIndexBuffer->create();
    VertexIndexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    VertexIndexBuffer->bind();
    VertexIndexBuffer->allocate(object3d.Index.data(), object3d.Index.size() * sizeof(QVector3D));
    program->enableAttributeArray("indexes");
    program->setAttributeBuffer("indexes", GL_FLOAT, 0, 3);
    VertexIndexBuffer->release();

    QOpenGLBuffer *FaceIndexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    FaceIndexBuffer->create();
    FaceIndexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    FaceIndexBuffer->bind();
    FaceIndexBuffer->allocate(object3d.PerFaceData.data(), object3d.PerFaceData.size() * sizeof(QVector4D));
    program->enableAttributeArray("faceIndexes");
    program->setAttributeBuffer("faceIndexes", GL_FLOAT, 0, 4);
    FaceIndexBuffer->release();

    QVector3D points[256];

    points[0] = *(new QVector3D(1.0f,0.0f,0.0f));

    points[1] = *(new QVector3D(0.0f, 1.0f, 0.5f));
    points[2] = *(new QVector3D(1.0f, 0.0f, 1.0f));
    points[3] = *(new QVector3D(1.0f, 0.0f, 1.0f));
    points[4] = *(new QVector3D(0.0f, 1.0f, 0.0f));
    points[5] = *(new QVector3D(1.0f, 1.0f, 0.0f));
    points[6] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[7] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[8] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[9] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[10] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[11] = *(new QVector3D(1.0f, 0.0f, 0.0f));

    points[12] = *(new QVector3D(1.0f, 0.0f, 0.0f));
    points[13] = *(new QVector3D(1.0f, 0.0f, 0.0f));
    points[14] = *(new QVector3D(0.0f, 1.0f, 0.0f));
    points[15] = *(new QVector3D(0.0f, 1.0f, 0.0f));
    points[16] = *(new QVector3D(0.0f, 1.0f, 0.0f));

    points[17] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[18] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[19] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[20] = *(new QVector3D(1.0f, 0.0f, 0.0f));
    points[21] = *(new QVector3D(1.0f, 0.0f, 0.0f));
    points[22] = *(new QVector3D(1.0f, 0.0f, 0.0f));
    points[23] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[24] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[25] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[26] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[27] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[28] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[29] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[30] = *(new QVector3D(0.0f, 0.0f, 0.0f));


    points[31] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[32] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[33] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[34] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[35] = *(new QVector3D(0.3f, 0.7f, 0.0f));
    points[36] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[37] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[38] = *(new QVector3D(0.0f, 0.0f, 1.0f));
    points[39] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[40] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[41] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[42] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[43] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[44] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[45] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[46] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[47] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[48] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[49] = *(new QVector3D(0.0f, 0.0f, 0.0f));


    points[50] = *(new QVector3D(0.0f, 1.0f, 1.0f));
    points[51] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[52] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[53] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[54] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[55] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[56] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[57] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[58] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[59] = *(new QVector3D(1.0f, 1.0f, 1.0f));
    points[60] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[61] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[62] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[63] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[64] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[65] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[66] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[67] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[68] = *(new QVector3D(0.0f, 0.0f, 0.0f));
    points[69] = *(new QVector3D(0.0f, 0.0f, 0.0f));

    program->setUniformValueArray("points", points, 256);

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


void _gl_widget::pick(int Selection_position_x, int Selection_position_y)
{
    //makeCurrent();
    //
    //// Frame Buffer Object to do the off-screen rendering
    //context()->functions()->glGenFramebuffers(1, &FBO);
    //context()->functions()->glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    //
    //// Texture for drawing
    //glGenTextures(1,&Color_texture);
    //glBindTexture(GL_TEXTURE_2D,Color_texture);
    //// RGBA8
    //context()->extraFunctions()->glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8, this->Window->width(),this->Window->height());
    //// this implies that there is not mip mapping
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    //
    //// Texure for computing the depth
    //glGenTextures(1,&Depth_texture);
    //glBindTexture(GL_TEXTURE_2D,Depth_texture);
    //// Float
    //context()->extraFunctions()->glTexStorage2D(GL_TEXTURE_2D,1,GL_DEPTH_COMPONENT24, 300,300);
    //
    //// Attatchment of the textures to the FBO
    //context()->extraFunctions()->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,Color_texture,0);
    //context()->extraFunctions()->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,Depth_texture,0);
    //
    //// OpenGL will draw to these buffers (only one in this case)
    //static const GLenum Draw_buffers[]={GL_COLOR_ATTACHMENT0};
    //context()->extraFunctions()->glDrawBuffers(1,Draw_buffers);
    //
    //clear_window();
    //change_projection();
    //change_observer();
    //draw_objects();
    //
    // Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow !
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    glFlush();
    glFinish();

    // get the pixel
    int Color;
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&Color);


    uint B=uint((Color & 0x00FF0000) >> 16);
    uint G=uint((Color & 0x0000FF00) >> 8);
    uint R=uint((Color & 0x000000FF));

    int Selected_triangle= (R << 16) + (G << 8) + B;

    if (Selected_triangle==16777215) Selected_triangle=-1;

    qDebug() << Selection_position_x << Selection_position_y << "(" << R << G << B << ")" << Selected_triangle;
    draw_objects();

    glDeleteTextures(1,&Color_texture);
    glDeleteTextures(1,&Depth_texture);
    context()->functions()->glDeleteFramebuffers(1,&FBO);
    // the normal framebuffer takes the control of drawing
    context()->functions()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER,defaultFramebufferObject());
}
