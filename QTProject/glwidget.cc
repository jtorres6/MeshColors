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
#include <debugtools.h>

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

void _gl_widget::keyReleaseEvent(QKeyEvent *Keyevent)
{
    if(Keyevent->key() == Qt::Key_Control)
    {
        ControlPressed = false;
    }
}

void _gl_widget::keyPressEvent(QKeyEvent *Keyevent)
{
  switch(Keyevent->key()){
  case Qt::Key_Left:Light_angle_y+=10.0f*ANGLE_STEP;break;
  case Qt::Key_Right:Light_angle_y-=10.0f*ANGLE_STEP;break;
  case Qt::Key_Up:Light_angle_x+=10.0f*ANGLE_STEP;break;
  case Qt::Key_Down:Light_angle_x-=10.0f*ANGLE_STEP;break;
  case Qt::Key_PageUp:Light_distance*=1.2;break;
  case Qt::Key_PageDown:Light_distance/=1.2;break;

  case Qt::Key_V:ColorLerpEnabled=!ColorLerpEnabled;
      break;

  case Qt::Key_R:TriangleSelectionMode=!TriangleSelectionMode;
      break;

  case Qt::Key_M:LightingEnabled=!LightingEnabled;
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

  case Qt::Key_Semicolon:
      DrawingSamplesID = false;

      // TO-DO: This can be One method.
      object3d.UpdateMeshColorsArray(object3d.points);
      UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  case Qt::Key_Comma:
      DrawingSamplesID = true;
      object3d.UpdateMeshColorsArray(object3d.selectionPoints);
      UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  case Qt::Key_Control:
      ControlPressed = true;
      break;

  case Qt::Key_F:
      Observer_pos_x = 0.0f;
      Observer_pos_y = 0.0f;
      Observer_angle_y = 0.0f;
      Observer_angle_x = 0.0f;
      Observer_distance = 10.0f;
      break;
  }


  update();
}


void _gl_widget::MoveCameraRightLeft(QPair<qint32, qint32> InUnits)
{
    if(ControlPressed)
    {
        Observer_pos_x+=0.005f*InUnits.first;
        Observer_pos_y-=0.005f*InUnits.second;
    }
    else
    {
        Observer_angle_y+=ANGLE_STEP*InUnits.first;
        Observer_angle_x+=ANGLE_STEP*InUnits.second;
    }
    update();
}

void _gl_widget::AddCameraZoom(const float InValue)
{
    Observer_distance+=(0.005f*(Observer_distance/20.0f))*InValue;
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
    Translation.translate(Observer_pos_x, Observer_pos_y, -Observer_distance);

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
    Projection_light  = QMatrix4x4();
    Rotation_x_light  = QMatrix4x4();
    Rotation_y_light  = QMatrix4x4();
    Translation_light = QMatrix4x4();

    const float AspectRatio = GLfloat(width()) / GLfloat(height());
    Projection_light.frustum(X_MIN * AspectRatio, X_MAX * AspectRatio, Y_MIN, Y_MAX , FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);
    Rotation_x_light.rotate(Light_angle_x, 1, 0, 0);
    Rotation_y_light.rotate(Light_angle_y, 0, 1, 0);
    Translation_light.translate(0, 0, -Light_distance);

    Projection_light*=Translation_light;
    Projection_light*=Rotation_x_light;
    Projection_light*=Rotation_y_light;

    LightPosition = QVector4D(1.0f,1.0f, 1.0f,1.0f) * Projection_light;
}


/*****************************************************************************//**
 * Funcion que dibuja los objetos
 *
 *
 *
 *****************************************************************************/

void _gl_widget::draw_objects()
{
    int matrixLocation = program->uniformLocation("matrix");

    if(!TriangleSelectionMode)
    {
        program->bind();

        program->setUniformValue(matrixLocation, Projection);

        // Draw 3D model
        VAO->bind();

        program->setUniformValue("BaseRendering", false);
        glLineWidth(2.0f);

        program->setUniformValue("LightPos", LightPosition);

        program->setUniformValue("ColorLerpEnabled", !DrawingSamplesID && ColorLerpEnabled);
        program->setUniformValue("LightingEnabled", !DrawingSamplesID && LightingEnabled);

        context->extraFunctions()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glDrawArrays(GL_TRIANGLES, 0, object3d.VerticesDrawArrays.size());
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

        VAO->release();
        program->release();
    }
    else
    {
        program2->bind();
        VAO2->bind();

        int matrixLocation = program2->uniformLocation("matrix");
        program2->setUniformValue(matrixLocation, Projection);

        program->setUniformValue("ColorLerpEnabled", false);
        program->setUniformValue("LightingEnabled", false);
        program2->setUniformValue("LineColor", QVector4D(0.0f, 0.3f, 0.8f, 1.0f));
        program2->setUniformValue("LineMode", false);

        glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());

        VAO2->release();
        program2->release();
    }

    program2->bind();

    // Draw axis
    VAO3->bind();
    glLineWidth(1.0f);

    matrixLocation = program2->uniformLocation("matrix");
    program2->setUniformValue(matrixLocation, Projection);
    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.5f, 1.0f, 1.0f));
    program2->setUniformValue("LineMode", false);

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
    VAO2->bind();
    draw_objects();
    //int matrixLocation = program2->uniformLocation("matrix");
    //program2->setUniformValue(matrixLocation, Projection);
    //
    //program->setUniformValue("ColorLerpEnabled", false);
    //program->setUniformValue("LightingEnabled", false);
    //
    //program2->setUniformValue("LineColor", QVector4D(0.0f, 0.3f, 0.8f, 1.0f));
    //program2->setUniformValue("LineMode", false);
    //
    //glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());
    //
    //VAO2->release();
    //program2->release();
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
  DebugTools::Update();
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
    makeCurrent();
    Axis = _axis(1500.0f);

    object3d = _object3D(ModelFilePath);

    LoadProgram();

    CreateBuffers();

    LogGlInfo();

    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);

    Observer_angle_x=0;
    Observer_angle_y=0;
    Observer_distance=DEFAULT_DISTANCE;

    Light_angle_x=0;
    Light_angle_y=0;
    Light_distance=DEFAULT_DISTANCE;
}


void _gl_widget::pick(int Selection_position_x, int Selection_position_y)
{
    if(program == nullptr)
        return;

    program->bind();
    program->setUniformValue("ColorLerpEnabled", false);
    program->setUniformValue("LightingEnabled", false);
    program->release();

    QList<int> Indexes;

    if(!TriangleSelectionMode)
    {
        DrawingSamplesID = true;

        makeCurrent();

        object3d.UpdateMeshColorsArray(object3d.selectionPoints);

        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

        update();
        paintGL();

        // get the pixel
        unsigned char data[PencilSize*PencilSize][4];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y, PencilSize, PencilSize, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Convert the color back to an integer ID
        for(int i = 0; i < PencilSize*PencilSize; i++)
        {
            int pickedID =
                data[i][0] +
                data[i][1] * 256 +
                data[i][2] * 256 * 256;

            if(pickedID != -1 && pickedID < object3d.points.size())
            {
                object3d.points[pickedID] = QVector4D(CurrentPaintingColor.red()/255.0f, CurrentPaintingColor.green()/255.0f, CurrentPaintingColor.blue()/255.0f, CurrentPaintingColor.alpha()/255.0f);

                if(!Indexes.contains(pickedID))
                {
                    DebugTools::DrawDebugString(Window, "#\n|\n|\nSelected index --> " + QString::number(pickedID) + " (" + QString::number(data[i][0]) + ", " +
                            QString::number(data[i][1])+ ", " +
                             QString::number(data[i][2]) + ")"+ "\n Pos: " + QString::number(Selection_position_x) + " " + QString::number(Selection_position_y),
                                     Selection_position_x, Window->height() - Selection_position_y, 500, 100,
                                     "QLabel { color : red; }", 0.1f);
                    Indexes.append(i);
                }
            }
        }

        object3d.UpdateMeshColorsArray(object3d.points);
        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        DrawingSamplesID = false;
    }
    else
    {
        makeCurrent();
        clear_window();

        //paintGL();
        DrawTrianglesSelectionMode();

        // get the pixel
        unsigned char data[4];
        int Color;
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(Selection_position_x,Selection_position_y,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&Color);
        uint R = ((Color & 0x000000FF));
        uint G = ((Color & 0x0000FF00) >> 8);
        uint B = ((Color & 0x00FF0000) >> 16);

        int pickedID =
           R +
           G * 256 +
           B * 256 * 256;
            //data[0] +
            //data[1] * 256 +
            //data[2] * 256 * 256;

        if(pickedID != -1)
        {
            DebugTools::DrawDebugString(Window, "#\n|\n|\n Previous --> " +   QString::number(SelectedTriangle) + "\n Selected index --> " + QString::number(pickedID) + " = " + QString::number(R) + " + " +
                    QString::number(G)+ " * 256 + " +
                     QString::number(B) + " * 256 * 256"+ "\n Pos: " + QString::number(Selection_position_x) + " " + QString::number(Selection_position_y),
                             Selection_position_x, Window->height() - Selection_position_y, 500, 100,
                             "QLabel { color : red; }", 0.1f);
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
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, InSsbo);
    context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, InSize, InData, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}

void _gl_widget::SetObjectPath(const char* InNewPath)
{
    ModelFilePath = InNewPath;
    initializeGL();
}

void _gl_widget::SetMeshColorsArray(QVector<QVector4D> InColors)
{
    object3d.points = InColors;
    object3d.UpdateMeshColorsArray(InColors);
    UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

void _gl_widget::SetResolutionsArray(QVector<int> InRes)
{
    object3d.Resolutions = InRes;
    object3d.UpdateResolutionsArray(InRes);
    UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

const QVector<QVector4D>& _gl_widget::GetMeshColorsArray() const
{
    return object3d.points;
}

const QVector<int>& _gl_widget::GetResolutionsArray() const
{
    return object3d.Resolutions;
}

const _object3D *_gl_widget::GetObject3D() const
{
    return &object3d;
}

void _gl_widget::SetCurrentPaintingColor(const QColor &InNewColor)
{
    if(InNewColor.isValid())
    {
        CurrentPaintingColor = InNewColor;
    }
}

void _gl_widget::IncrementResolution()
{
    if(SelectedTriangle >= 0 && SelectedTriangle < object3d.Resolutions.size() && object3d.Resolutions[SelectedTriangle] < 32)
    {
        object3d.Resolutions[SelectedTriangle] *= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        update();
    }
}

void _gl_widget::DecreaseResolution()
{
    if(SelectedTriangle >= 0 && SelectedTriangle < object3d.Resolutions.size() && SelectedTriangle >= 0 && object3d.Resolutions[SelectedTriangle] > 2)
    {
        object3d.Resolutions[SelectedTriangle] /= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        update();
    }
}

void _gl_widget::EnableTriangleSelectionMode()
{
    TriangleSelectionMode = true;
    update();
}

void _gl_widget::UpdatePencilSize(const int InValue)
{
    PencilSize = InValue;
}

void _gl_widget::ToggleLighting()
{
    LightingEnabled = !LightingEnabled;
    update();
}

void _gl_widget::ToggleColorInterpolation()
{
    ColorLerpEnabled = !ColorLerpEnabled;
    update();
}

void _gl_widget::LoadProgram()
{
    context = new QOpenGLContext(this);
    program = new QOpenGLShaderProgram(context);

    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   "../QTProject/MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "../QTProject/MeshColorsGeometry.gsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../QTProject/MeshColorsFragment.fsh");
    program->link();
}

void _gl_widget::CreateBuffers()
{
    program->bind();

    VAO = new QOpenGLVertexArrayObject();
    VAO->create();
    VAO->bind();

    InitializeBuffer(program, object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    InitializeBuffer(program, object3d.Colors.data(), object3d.Colors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);
    InitializeBuffer(program,object3d.Index.data(), object3d.Index.size() * sizeof(QVector3D), "indexes", GL_FLOAT, 0, 3);

    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);

    context->functions()->glGenBuffers(1, &ssbo);

    UpdateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

    VAO->release();
    program->release();

    program2 = new QOpenGLShaderProgram(context);

    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   "../QTProject/BaseVertex.vsh");
    program2->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../QTProject/BaseFragment.fsh");
    program2->link();
    program2->bind();

    VAO2 = new QOpenGLVertexArrayObject();
    VAO2->create();
    VAO2->bind();

    InitializeBuffer(program2, object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    InitializeBuffer(program2, object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

    VAO2->release();

    VAO3 = new QOpenGLVertexArrayObject();
    VAO3->create();
    VAO3->bind();

    InitializeBuffer(program2, Axis.Vertices.data(), Axis.Vertices.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    InitializeBuffer(program2, Axis.Colors.data(), Axis.Colors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

    VAO3->release();
    program2->release();
}

void _gl_widget::LogGlInfo()
{
    const GLubyte* strm;
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
}

void _gl_widget::InitializeBuffer(QOpenGLShaderProgram* InShader, void* InData, const int InSize, const char* InName, const GLenum InType, const int InOffset, const int InStride)
{
    QOpenGLBuffer *buffer = GenerateBuffer(InData, InSize);
    buffer->bind();
    InShader->enableAttributeArray(InName);
    InShader->setAttributeBuffer(InName, InType, InOffset, InStride);
    buffer->release();
}
