 /*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */

#include "glwidget.h"
#include "file_ply_stl.h"
#include "window.h"
#include <QByteArray>
#include <QColor>
#include <QOpenGLTexture>
#include <QColorDialog>
#include <QtMath>

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
  camera = MovableObject();
  light = MovableObject();
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
  case Qt::Key_Left:lightAngleY+=10.0f*ANGLE_STEP;break;
  case Qt::Key_Right:lightAngleY-=10.0f*ANGLE_STEP;break;
  case Qt::Key_Up:lightAngleX+=10.0f*ANGLE_STEP;break;
  case Qt::Key_Down:lightAngleX-=10.0f*ANGLE_STEP;break;
  case Qt::Key_PageUp:lightDistance*=1.2;break;
  case Qt::Key_PageDown:lightDistance/=1.2;break;

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

      if(SelectedTriangleID >= 0 && object3d.Resolutions[SelectedTriangleID] < MAX_SAMPLES)
      {
          object3d.Resolutions[SelectedTriangleID] *= 2;
          object3d.UpdateResolutionsArray(object3d.Resolutions);
          updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      }
      break;

  case Qt::Key_Semicolon:
      DrawingSamplesID = false;

      // TO-DO: This can be One method.
      object3d.UpdateMeshColorsArray(object3d.points);
      updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  case Qt::Key_Comma:
      DrawingSamplesID = true;
      object3d.UpdateMeshColorsArray(object3d.selectionPoints);
      updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
      break;

  case Qt::Key_Control:
      ControlPressed = true;
      break;

  case Qt::Key_Shift:
      ShiftPressed = true;
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


void _gl_widget::moveCameraRightLeft(QPair<qint32, qint32> InUnits)
{
    if(ControlPressed)
    {
        Observer_pos_x += 0.005f*InUnits.first;
        Observer_pos_y -= 0.005f*InUnits.second;
    }
    else
    {
        Observer_angle_y += ANGLE_STEP*InUnits.first;
        Observer_angle_x += ANGLE_STEP*InUnits.second;
    }
    update();
}

void _gl_widget::addCameraZoom(const float InValue)
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
    camera.reset();

    const float AspectRatio = GLfloat(width()) / GLfloat(height());
    camera.projection.frustum(X_MIN * AspectRatio, X_MAX * AspectRatio, Y_MIN, Y_MAX , FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);
    camera.move(Observer_pos_x, Observer_pos_y, -Observer_distance);
    camera.rotate(Observer_angle_x, 1, 0, 0);
    camera.rotate(Observer_angle_y, 0, 1, 0);
}

/*****************************************************************************//**
 * Funcion para definir la transformación de vista (posicionar la camara)
 *
 *
 *
 *****************************************************************************/

void _gl_widget::change_observer()
{
    light.reset();
    light.move(0, 0, -lightDistance);
    light.rotate(lightAngleX, 1, 0, 0);
    light.rotate(lightAngleY, 0, 1, 0);
}


/*****************************************************************************//**
 * Funcion que dibuja los objetos
 *
 *
 *
 *****************************************************************************/

void _gl_widget::draw_objects()
{
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    int matrixLocation = program->uniformLocation("matrix");

    program->bind();

    program->setUniformValue(matrixLocation, camera.getProjectedTransform());

    // Draw 3D model
    VAO->bind();

    program->setUniformValue("BaseRendering", false);
    glLineWidth(1.5f);

    program->setUniformValue("LightPos", light.getLocation());

    program->setUniformValue("ColorLerpEnabled", !DrawingSamplesID && ColorLerpEnabled);
    program->setUniformValue("LightingEnabled", !DrawingSamplesID && LightingEnabled);

    context->extraFunctions()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glDrawArrays(GL_TRIANGLES, 0, object3d.VerticesDrawArrays.size());
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    VAO->release();
    program->release();

    if(wireframeMode) {
        program2->bind();
        VAO4->bind();

        matrixLocation = program2->uniformLocation("matrix");
        program2->setUniformValue(matrixLocation, camera.getProjectedTransform());
        program2->setUniformValue("LineColor", QVector4D(0.0f, 0.3f, 0.8f, 0.5f));
        program2->setUniformValue("LineMode", true);

        glDrawArrays(GL_LINES,0, object3d.TrianglesDrawArrays.size());

        VAO4->release();
        program2->release();

    }

    program2->bind();
    // Draw axis
    VAO3->bind();
    glLineWidth(1.0f);

    matrixLocation = program2->uniformLocation("matrix");
    program2->setUniformValue(matrixLocation, camera.getProjectedTransform());
    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.7f, 1.0f, 1.0f));
    program2->setUniformValue("LineMode", false);

    glDrawArrays(GL_LINES, 0, Axis.vertices.size());

    VAO3->release();

    if(VAO5 != nullptr && SelectedTriangleDrawArray.size() > 0)
    {
        // Draw axis
        VAO5->bind();
        glLineWidth(7.5f);

        matrixLocation = program2->uniformLocation("matrix");
        program2->setUniformValue(matrixLocation, camera.getProjectedTransform());
        program2->setUniformValue("LineColor", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
        program2->setUniformValue("LineMode", true);

        glDrawArrays(GL_LINES, 0, SelectedTriangleDrawArray.size());

        VAO5->release();
    }

    program2->release();
}

void _gl_widget::drawTrianglesSelectionMode()
{
    clear_window();
    change_projection();
    change_observer();

    program2->bind();
    VAO2->bind();

    const int matrixLocation = program2->uniformLocation("matrix");
    program2->setUniformValue(matrixLocation, camera.getProjectedTransform());

    program->setUniformValue("ColorLerpEnabled", false);
    program->setUniformValue("LightingEnabled", false);
    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.3f, 0.8f, 1.0f));
    program2->setUniformValue("LineMode", false);

    glDrawArrays(GL_TRIANGLES,0, object3d.VerticesDrawArrays.size());

    VAO2->release();
    program2->release();

    program2->bind();

    // Draw axis
    VAO3->bind();
    glLineWidth(1.0f);

    program2->setUniformValue(matrixLocation, camera.getProjectedTransform());
    program2->setUniformValue("LineColor", QVector4D(0.0f, 0.7f, 1.0f, 1.0f));
    program2->setUniformValue("LineMode", false);

    glDrawArrays(GL_LINES, 0, Axis.vertices.size());

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
    makeCurrent();
    Axis = _axis(1500.0f);

    QVector<QVector3D> triangles;
    QVector<QVector3D> vertices;
    if(!ModelFilePath.empty())
    {
        readPlyFile(ModelFilePath, vertices, triangles);
    }
    object3d = _object3D(vertices, triangles);

    SelectedTriangleDrawArray.clear();
    TriangleSelectionMode = false;

    loadProgram();

    createBuffers();

    logGlInfo();

    glClearColor(0.22f, 0.22f, 0.22f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    Observer_angle_x=0;
    Observer_angle_y=0;
    Observer_distance=DEFAULT_DISTANCE;

    lightAngleX=0;
    lightAngleY=0;
    lightDistance=DEFAULT_DISTANCE;
}

bool _gl_widget::readPlyFile(const string &filename, QVector<QVector3D>& outVertices, QVector<QVector3D>& outTriangles)
 {
     _file_ply ply;
     if(ply.open(filename) == 0) return false;

     vector<float> Coordinates;
     vector<unsigned int> Positions;
     ply.read(Coordinates, Positions);

     for(size_t i = 0; i <= Coordinates.size()-3; i+=3)
     {
         outVertices.push_back(QVector3D(Coordinates[i],Coordinates[i+1],Coordinates[i+2]));
     }

     for(size_t i = 0; i <= Positions.size()-3; i+=3)
     {
         outTriangles.push_back(QVector3D(Positions[i],Positions[i+1],Positions[i+2]));
     }

     return true;
 }


void _gl_widget::pick(const int Selection_position_x, const int Selection_position_y)
{
    if (program == nullptr) return;

    program->bind();
    program->setUniformValue("ColorLerpEnabled", false);
    program->setUniformValue("LightingEnabled", false);
    program->release();

    QList<int> Indexes;

    if (!TriangleSelectionMode) {
        DrawingSamplesID = true;

        makeCurrent();

        object3d.UpdateMeshColorsArray(object3d.selectionPoints);

        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

        update();
        paintGL();

        // get the pixel
        unsigned char data[PencilSize*PencilSize][4];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);

        int PosX = Selection_position_x - 12.5 - (PencilSize*0.5f);
        int PosY = Selection_position_y - 12.5 - (PencilSize*0.5f);

        glReadPixels(PosX, PosY, PencilSize, PencilSize, GL_RGBA, GL_UNSIGNED_BYTE, data);

        QMap<int, float> SelectedIDs;

        // Convert the color back to an integer ID
        for(int i = 0; i < PencilSize*PencilSize; i++) {
            const int pickedID =
                data[i][0] +
                data[i][1] * 256 +
                data[i][2] * 256 * 256;

            const float DistanceToCenter = qFabs(0.5f - i/(PencilSize*PencilSize));

             if (data[3] != 0 && pickedID != -1 && pickedID < object3d.points.size()) {
                if (!SelectedIDs.contains(pickedID)) {
                    SelectedIDs.insert(pickedID, DistanceToCenter);
                    object3d.points[pickedID] =  (1.0f- PencilTransparency) * object3d.points[pickedID] +  PencilTransparency * QVector4D(CurrentPaintingColor.red()/255.0f, CurrentPaintingColor.green()/255.0f, CurrentPaintingColor.blue()/255.0f, CurrentPaintingColor.alpha()/255.0f);

                    if (!Indexes.contains(pickedID)) {
                        Indexes.append(i);
                        qDebug() << pickedID;
                    }
                }
                else if (SelectedIDs.find(pickedID).value() > DistanceToCenter) {
                    SelectedIDs.find(pickedID).value() = DistanceToCenter;
                    object3d.points[pickedID] = DistanceToCenter * object3d.points[pickedID] + (1.0f - DistanceToCenter) * QVector4D(CurrentPaintingColor.red()/255.0f, CurrentPaintingColor.green()/255.0f, CurrentPaintingColor.blue()/255.0f, CurrentPaintingColor.alpha()/255.0f);
                }
            }
        }

        object3d.UpdateMeshColorsArray(object3d.points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        DrawingSamplesID = false;
    }
    else {
        makeCurrent();
        clear_window();

        //paintGL();
        drawTrianglesSelectionMode();

        // get the pixel
        unsigned char data[4];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(Selection_position_x, Selection_position_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);

        int pickedID =
            data[0] +
            data[1] * 256 +
            data[2] * 256 * 256;

        if(data[3] != 0 && pickedID >= 0 && SelectedTriangleID < object3d.Resolutions.size()) {
            SelectedTriangleID = pickedID;
            SelectedTriangle = object3d.Triangles[SelectedTriangleID];

            VAO5 = new QOpenGLVertexArrayObject();
            VAO5->create();
            VAO5->bind();

            SelectedTriangleDrawArray.clear();

            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);
            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
            SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);

            initializeBuffer(program2, SelectedTriangleDrawArray.data(), SelectedTriangleDrawArray.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
            initializeBuffer(program2, new QVector4D(1.0f, 0.0f, 0.0f, 1.0f), sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

            VAO5->release();
        }

        TriangleSelectionMode = false;
    }

    program->bind();
    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
    program->setUniformValue("LightingEnabled", true);
    program->release();
}

void _gl_widget::selectTriangle(const int Selection_position_x, const int Selection_position_y)
{
    if (program == nullptr) return;

    program->bind();
    program->setUniformValue("ColorLerpEnabled", false);
    program->setUniformValue("LightingEnabled", false);
    program->release();

    makeCurrent();
    clear_window();

    //paintGL();
    drawTrianglesSelectionMode();

    // get the pixel
    unsigned char data[4];
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(Selection_position_x, Selection_position_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);

    int pickedID =
        data[0] +
        data[1] * 256 +
        data[2] * 256 * 256;

    if(data[3] != 0 && pickedID >= 0 && pickedID < object3d.Triangles.size()) {
        SelectedTriangleID = pickedID;
        SelectedTriangle = object3d.Triangles[SelectedTriangleID];

        VAO5 = new QOpenGLVertexArrayObject();
        VAO5->create();
        VAO5->bind();

        SelectedTriangleDrawArray.clear();

        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);

        initializeBuffer(program2, SelectedTriangleDrawArray.data(), SelectedTriangleDrawArray.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
        initializeBuffer(program2, new QVector4D(1.0f, 0.0f, 0.0f, 1.0f), sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

        VAO5->release();
    }

    TriangleSelectionMode = false;

    program->bind();
    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
    program->setUniformValue("LightingEnabled", true);
    program->release();

    update();
    paintGL();
}

QOpenGLBuffer* _gl_widget::generateBuffer(const void *InData, int InCount)
{
    QOpenGLBuffer* Buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    Buffer->create();
    Buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    Buffer->bind();
    Buffer->allocate(InData, InCount);
    Buffer->release();

    return Buffer;
}

void _gl_widget::updateSSBO(GLuint InSsbo, GLsizei InSize, void* InData)
{
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, InSsbo);
    context->functions()->glBufferData(GL_SHADER_STORAGE_BUFFER, InSize, InData, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}

void _gl_widget::setObjectPath(const char* InNewPath)
{
    ModelFilePath = InNewPath;

    initializeGL();
}

void _gl_widget::setMeshColorsArray(QVector<QVector4D> InColors)
{
    object3d.points = InColors;
    object3d.UpdateMeshColorsArray(InColors);
    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

void _gl_widget::setResolutionsArray(QVector<int> InRes)
{
    object3d.Resolutions = InRes;
    object3d.UpdateResolutionsArray(InRes);
    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

const QVector<QVector4D>& _gl_widget::getMeshColorsArray() const
{
    return object3d.points;
}

const QVector<int>& _gl_widget::getResolutionsArray() const
{
    return object3d.Resolutions;
}

const _object3D* _gl_widget::getObject3D() const
{
    return &object3d;
}

void _gl_widget::setCurrentPaintingColor(const QColor &InNewColor)
{
    if(InNewColor.isValid())
    {
        CurrentPaintingColor = InNewColor;
    }
}

void _gl_widget::incrementResolution()
{
    if(SelectedTriangleID >= 0 && SelectedTriangleID < object3d.Resolutions.size() && object3d.Resolutions[SelectedTriangleID] < MAX_SAMPLES)
    {
        object3d.Resolutions[SelectedTriangleID] *= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        object3d.UpdateMeshColorsArray(object3d.points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        update();
    }
}

void _gl_widget::decreaseResolution()
{
    if(SelectedTriangleID >= 0 && SelectedTriangleID < object3d.Resolutions.size() && SelectedTriangleID >= 0 && object3d.Resolutions[SelectedTriangleID] > 1)
    {
        object3d.Resolutions[SelectedTriangleID] /= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        object3d.UpdateMeshColorsArray(object3d.points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        update();
    }
}

void _gl_widget::enableTriangleSelectionMode()
{
    TriangleSelectionMode = !TriangleSelectionMode;
    update();
}

void _gl_widget::setTriangleSelectionMode(bool active)
{
    TriangleSelectionMode = active;
    update();
}

void _gl_widget::updatePencilSize(const int InValue)
{
    PencilSize = InValue;
}

void _gl_widget::updatePencilTransparency(const int InValue)
{
    PencilTransparency = float(InValue)/100.0f;
}

void _gl_widget::toggleLighting()
{
    LightingEnabled = !LightingEnabled;
    update();
}

void _gl_widget::toggleColorInterpolation()
{
    ColorLerpEnabled = !ColorLerpEnabled;
    update();
}

void _gl_widget::toggleWireframeMode()
{
    wireframeMode = !wireframeMode;
    update();
}

void _gl_widget::loadProgram()
{
    context = new QOpenGLContext(this);
    program = new QOpenGLShaderProgram(context);

    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   "../QTProject/MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "../QTProject/MeshColorsGeometry.gsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../QTProject/MeshColorsFragment.fsh");
    program->link();
}

void _gl_widget::createBuffers()
{
    program->bind();

    VAO = new QOpenGLVertexArrayObject();
    VAO->create();
    VAO->bind();

    initializeBuffer(program, object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    initializeBuffer(program, object3d.colors.data(), object3d.colors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);
    initializeBuffer(program, object3d.Index.data(), object3d.Index.size() * sizeof(QVector3D), "indexes", GL_FLOAT, 0, 3);
    initializeBuffer(program, object3d.VerticesNormals.data(), object3d.VerticesNormals.size() * sizeof(QVector4D), "normals", GL_FLOAT, 0, 4);

    program->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);

    context->functions()->glGenBuffers(1, &ssbo);

    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

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

    initializeBuffer(program2, object3d.VerticesDrawArrays.data(), object3d.VerticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    initializeBuffer(program2, object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

    VAO2->release();

    VAO3 = new QOpenGLVertexArrayObject();
    VAO3->create();
    VAO3->bind();

    initializeBuffer(program2, Axis.vertices.data(), Axis.vertices.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    initializeBuffer(program2, Axis.colors.data(), Axis.colors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

    VAO3->release();
    program2->release();

    VAO4 = new QOpenGLVertexArrayObject();
    VAO4->create();
    VAO4->bind();

    initializeBuffer(program2, object3d.TrianglesDrawArrays.data(), object3d.TrianglesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
    initializeBuffer(program2, object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

    VAO4->release();

}

void _gl_widget::logGlInfo()
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

void _gl_widget::initializeBuffer(QOpenGLShaderProgram* InShader, void* InData, const int InSize, const char* InName, const GLenum InType, const int InOffset, const int InStride)
{
    QOpenGLBuffer *buffer = generateBuffer(InData, InSize);
    buffer->bind();
    InShader->enableAttributeArray(InName);
    InShader->setAttributeBuffer(InName, InType, InOffset, InStride);
    buffer->release();
}
