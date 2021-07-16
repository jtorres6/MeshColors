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
#include <QColorDialog>

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
  case Qt::Key_V:ColorLerpEnabled=!ColorLerpEnabled;
      break;

  case Qt::Key_R:TriangleSelectionMode=!TriangleSelectionMode;
      break;

  case Qt::Key_C:
  {
        QColor NewColor = QColorDialog::getColor(Qt::yellow, this );

        if(NewColor.isValid())
        {
            CurrentPaintingColor = NewColor;
        }

        break;
  }

  case Qt::Key_Plus:

      if(SelectedTriangle >= 0 && object3d.Resolutions[SelectedTriangle] < 64)
      {
          object3d.Resolutions[SelectedTriangle] *= 2;
          object3d.UpdateResolutionsArray(object3d.Resolutions);
          UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      }
      break;

  case Qt::Key_Minus:

      if(SelectedTriangle >= 0 && object3d.Resolutions[SelectedTriangle] > 2)
      {
          object3d.Resolutions[SelectedTriangle] /= 2;
          object3d.UpdateResolutionsArray(object3d.Resolutions);
          UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      }
      break;

  case Qt::Key_Semicolon:
      object3d.UpdateMeshColorsArray(object3d.points);
      UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  case Qt::Key_Comma:
      object3d.UpdateMeshColorsArray(object3d.selectionPoints);
      UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  }

  update();
}


void _gl_widget::MoveCameraRightLeft(QPair<qint32, qint32> InUnits)
{
    Observer_angle_y+=ANGLE_STEP*InUnits.first;
    Observer_angle_x+=ANGLE_STEP*InUnits.second;
    update();
}


void _gl_widget::AddCameraZoom(const float InValue)
{
    Observer_distance+=0.005f*InValue;
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
    Projection  = QMatrix4x4();
    Rotation_x  = QMatrix4x4();
    Rotation_y  = QMatrix4x4();
    Translation = QMatrix4x4();

    const float AspectRatio = GLfloat(width()) / GLfloat(height());
    Projection.frustum(X_MIN * AspectRatio, X_MAX * AspectRatio, Y_MIN, Y_MAX , FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);
    Rotation_x.rotate(Observer_angle_x, 1, 0, 0);
    Rotation_y.rotate(Observer_angle_y, 0, 1, 0);
    Translation.translate(0, 0, -Observer_distance);

    Projection*=Translation;
    Projection*=Rotation_x;
    Projection*=Rotation_y;
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
    program->bind();

    int matrixLocation = program->uniformLocation("matrix");
    program->setUniformValue(matrixLocation, Projection);

    // Draw 3D model
    VAO->bind();

    program->setUniformValue("BaseRendering", false);
    glLineWidth(2.0f);

    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
    program->setUniformValue("LightPos", LightPosition);
    program->setUniformValue("LightingEnabled", true);
    context->extraFunctions()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    VAO->release();
    program->release();

    // Draw wireframe
    program2->bind();
    VAO2->bind();

    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.5f, 1.0f, 1.0f));
    program2->setUniformValue("LineMode", true);
    program2->setUniformValue(matrixLocation, Projection);

    glDrawArrays(GL_LINES, 0, object3d.VerticesDrawArrays.size());

    VAO2->release();
    program2->release();

    // Draw wireframe
    program2->bind();
    VAO3->bind();

    glLineWidth(1.0f);

    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.5f, 1.0f, 1.0f));
    program2->setUniformValue("LineMode", false);
    program2->setUniformValue(matrixLocation, Projection);

    glDrawArrays(GL_LINES, 0, Axis.Vertices.size());

    VAO3->release();
    program2->release();
}

void _gl_widget::DrawTrianglesSelectionMode()
{
    clear_window();
    change_projection();
    change_observer();

    program2->bind();
    // Draw 3D model
    VAO3->bind();

    int matrixLocation2 = program2->uniformLocation("matrix");
    program2->setUniformValue(matrixLocation2, Projection);

    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.3f, 0.8f, 1.0f));
    program2->setUniformValue("LineMode", false);

    glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());

    VAO3->release();
    program2->release();
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
    Axis = _axis(1500.0f);

    object3d = _object3D(p);

    program = new QOpenGLShaderProgram(context);

    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "MeshColorsGeometry.gsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"MeshColorsFragment.fsh");
    program->link();
    program->bind();

    // VAO 2
    VAO = new QOpenGLVertexArrayObject();
    VAO->create();
    VAO->bind();

    QOpenGLBuffer *positionBuffer = GenerateBuffer(object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D));
    positionBuffer->bind();
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    QOpenGLBuffer *colorBuffer = GenerateBuffer(object3d.Colors.data(), object3d.Colors.size() * sizeof(QVector4D));
    colorBuffer->bind();
    program->enableAttributeArray("color");
    program->setAttributeBuffer("color", GL_FLOAT, 0, 4);
    colorBuffer->release();

    QOpenGLBuffer *VertexIndexBuffer = GenerateBuffer(object3d.Index.data(), object3d.Index.size() * sizeof(QVector3D));
    VertexIndexBuffer->bind();
    program->enableAttributeArray("indexes");
    program->setAttributeBuffer("indexes", GL_FLOAT, 0, 3);
    VertexIndexBuffer->release();

    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);

    context->functions()->glGenBuffers(1, &ssbo);

    UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

    VAO->release();
    program->release();

    program2 = new QOpenGLShaderProgram(context);

    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "BaseVertex.vsh");
    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"BaseFragment.fsh");
    program2->link();
    program2->bind();

    // VAO 3
    VAO2 = new QOpenGLVertexArrayObject();
    VAO2->create();
    VAO2->bind();

    positionBuffer = GenerateBuffer(object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D));
    positionBuffer->bind();
    program2->enableAttributeArray("vertex");
    program2->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer = GenerateBuffer(object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D));
    colorBuffer->bind();
    program2->enableAttributeArray("color");
    program2->setAttributeBuffer("color", GL_FLOAT, 0, 4);
    colorBuffer->release();

    VAO2->release();

    // VAO 4
    VAO3 = new QOpenGLVertexArrayObject();
    VAO3->create();
    VAO3->bind();

    positionBuffer = GenerateBuffer(Axis.Vertices.data(), Axis.Vertices.size() * sizeof(QVector3D));
    positionBuffer->bind();
    program2->enableAttributeArray("vertex");
    program2->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    positionBuffer->release();

    colorBuffer = GenerateBuffer(Axis.Colors.data(), Axis.Colors.size() * sizeof(QVector4D));
    colorBuffer->bind();
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
    program->bind();
    program->setUniformValue("ColorLerpEnabled", false);
    program->setUniformValue("LightingEnabled", false);
    program->release();

    if(!TriangleSelectionMode)
    {
        makeCurrent();

        object3d.UpdateMeshColorsArray(object3d.selectionPoints);

        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

        update();
        paintGL();

        // get the pixel
        unsigned char data[4];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE, data);

        // Convert the color back to an integer ID
        int pickedID =
            data[0] +
            data[1] * 256 +
            data[2] * 256 * 256;

        if(pickedID != -1 && pickedID < object3d.points.size())
        {
            object3d.points[pickedID] = QVector4D(CurrentPaintingColor.red()/255.0f, CurrentPaintingColor.green()/255.0f, CurrentPaintingColor.blue()/255.0f, CurrentPaintingColor.alpha()/255.0f);
            qDebug() << pickedID << data[0] << data[1] << data[2];
        }

        object3d.UpdateMeshColorsArray(object3d.points);
        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
    }
    else
    {
        makeCurrent();
        clear_window();

        //paintGL();
        DrawTrianglesSelectionMode();

        // get the pixel
        int Color;
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&Color);

        uint R = ((Color & 0x000000FF));
        uint G = ((Color & 0x0000FF00) >> 8);
        uint B = ((Color & 0x00FF0000) >> 16);

        uint Selected_triangle= (R << 16) + (G << 8) + B;

        if (Selected_triangle==16777215) Selected_triangle=-1;

        // Convert the color back to an integer ID
        uint pickedID =
            R +
            G * 256 +
            B * 256 * 256;

        if(pickedID != -1)
        {
            SelectedTriangle = pickedID;
        }
        TriangleSelectionMode = false;
    }
    program->bind();
    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
    program->setUniformValue("LightingEnabled", true);
    program->release();
}

QOpenGLBuffer* _gl_widget::GenerateBuffer(const void *InData, int InCount)
{
    QOpenGLBuffer* Buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    Buffer->create();
    Buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    Buffer->bind();
    Buffer->allocate(InData, InCount);
    Buffer->release();
    return Buffer;
}


void _gl_widget::UpdateSSBO(GLuint InSsbo, GLsizei InSize, void* InData)
{
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, InSize, InData, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}
