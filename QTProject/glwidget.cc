/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */

#include "glwidget.h"
#include "window.h"
#include <QByteArray>
#include <QColor>
#include <QOpenGLTexture>

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

  case Qt::Key_R:TriangleSelectionMode=!TriangleSelectionMode;
      break;
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

    if(!TriangleSelectionMode)
    {
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
            context->extraFunctions()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
            context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());
            context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
        }

        VAO2->release();
        program->release();
    }
    else
    {
        program2->bind();
        // Draw 3D model
        VAO3->bind();

        int matrixLocation2 = program2->uniformLocation("matrix");
        program2->setUniformValue(matrixLocation2, Projection);

        glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());

        VAO3->release();
        program2->release();
    }
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
    context = new QOpenGLContext(this);

    makeCurrent();
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

    for(uint i = 0; i< 1024; i++)
    {
        // Convert "i", the integer mesh ID, into an RGB color
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;
        pointsIndex[i] = *(new QVector3D(r/255.0f, g/255.0f, b/255.0f));
    }

    for(uint i = 0; i< 1024; i++)
    {
        points[i] = *(new QVector3D(0.0f, 1.0f, 0.0f));
    }

    program->setUniformValueArray("points", points, 256);
    program->setUniformValue("ColorLerpEnabled", false);

    //qDebug() << object3d.MeshColorArray[0].Colors[0][0];

    for(int i = 0; i <object3d.MeshColorArray.size(); i++)
    {
        qDebug() << object3d.MeshColorArray[i].Resolution;
    }

    context->functions()->glGenBuffers(1, &ssbo);
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, object3d.SsboSize() * object3d.MeshColorArray.size(), object3d.MeshColorArray.data(), GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    VAO2->release();
    program->release();

    VAO->release();

    program2 = new QOpenGLShaderProgram(context);

    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "BaseVertex.vsh");
    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"BaseFragment.fsh");
    program2->link();
    program2->bind();

    // VAO 2
    VAO3 = new QOpenGLVertexArrayObject();
    VAO3->create();
    VAO3->bind();

    positionBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer->create();
    positionBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer->bind();
    positionBuffer->allocate(object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D));
    program2->enableAttributeArray("vertex");
    program2->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colorBuffer->create();
    colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    colorBuffer->bind();
    colorBuffer->allocate(object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D));
    program2->enableAttributeArray("color");
    program2->setAttributeBuffer("color", GL_FLOAT, 0, 4);
    colorBuffer->release();

    VAO3->release();
    program2->release();

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
    if(!TriangleSelectionMode)
    {
        makeCurrent();
        clear_window();

        program->bind();
        program->setUniformValueArray("points", pointsIndex, 512);
        program->setUniformValue("ColorLerpEnabled", false);
        object3d.UpdateMeshColorsArray(object3d.selectionPoints);

        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, object3d.SsboSize() * object3d.MeshColorArray.size(), object3d.MeshColorArray.data(), GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

        program->release();

        update();

        paintGL();
        // get the pixel
        int Color;
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&Color);

        uint R = uint((Color & 0x000000FF));
        uint G = uint((Color & 0x0000FF00) >> 8);
        uint B = uint((Color & 0x00FF0000) >> 16);

        uint Selected_triangle= (R << 16) + (G << 8) + B;

        if (Selected_triangle==16777215) Selected_triangle=-1;

        // Convert the color back to an integer ID
        uint pickedID =
            R +
            G * 256 +
            B * 256 * 256;

        if(pickedID != -1 && pickedID < 1024)
        {
            object3d.points[pickedID] = QVector4D(0.0f, 1.0f, 1.0f, 1.0f);
            qDebug() << pickedID;
        }

        object3d.UpdateMeshColorsArray(object3d.points);

        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, object3d.SsboSize() * object3d.MeshColorArray.size(), object3d.MeshColorArray.data(), GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

        program->bind();
        program->setUniformValueArray("points", points, 1024);
        //program->setUniformValue("ColorLerpEnabled", true);
        program->release();
    }
    else
    {
        makeCurrent();
        clear_window();

        paintGL();

        // get the pixel
        int Color;
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&Color);

        uint R = uint((Color & 0x000000FF));
        uint G = uint((Color & 0x0000FF00) >> 8);
        uint B = uint((Color & 0x00FF0000) >> 16);

        uint Selected_triangle= (R << 16) + (G << 8) + B;

        if (Selected_triangle==16777215) Selected_triangle=-1;

        // Convert the color back to an integer ID
        uint pickedID =
            R +
            G * 256 +
            B * 256 * 256;

        if(pickedID != -1 && pickedID < 1024)
        {
            object3d.Resolutions[pickedID] = 4;
            qDebug() << pickedID;
            TriangleSelectionMode = false;
        }
    }
}
