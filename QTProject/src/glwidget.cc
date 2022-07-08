#include "glwidget.h"
#include "file_ply_stl.h"
#include "window.h"
#include <QByteArray>
#include <QColor>
#include <QOpenGLTexture>
#include <QColorDialog>
#include <QtMath>

using namespace _gl_widget_ne;

_gl_widget::_gl_widget()
{
    setMinimumSize(300, 300);
    setFocusPolicy(Qt::StrongFocus);
    camera = MovableObject();
    light = MovableObject();
}

void _gl_widget::keyReleaseEvent(QKeyEvent *Keyevent)
{
    switch(Keyevent->key()){
    case Qt::Key_Shift:
        ShiftPressed = false;
        break;
    case Qt::Key_Control:
        ControlPressed = false;
        break;

    }
}

void _gl_widget::keyPressEvent(QKeyEvent *Keyevent)
{
    switch(Keyevent->key()){
    case Qt::Key_Left:lightAngleY += 10.0f * ANGLE_STEP; break;
    case Qt::Key_Right:lightAngleY -= 10.0f * ANGLE_STEP; break;
    case Qt::Key_Up:lightAngleX += 10.0f * ANGLE_STEP; break;
    case Qt::Key_Down:lightAngleX -= 10.0f * ANGLE_STEP; break;
    case Qt::Key_PageUp:lightDistance *= 1.2; break;
    case Qt::Key_PageDown:lightDistance /= 1.2; break;

    case Qt::Key_V:ColorLerpEnabled=!ColorLerpEnabled;
        break;

    case Qt::Key_R:TriangleSelectionMode=!TriangleSelectionMode;
        break;

    case Qt::Key_M:LightingEnabled=!LightingEnabled;
        break;

    case Qt::Key_C:
    {
        QColor NewColor = QColorDialog::getColor(Qt::yellow, this);

        if(NewColor.isValid()) {
            CurrentPaintingColor = NewColor;
        }

        break;
    }

    case Qt::Key_Plus:

        if(SelectedTriangleID >= 0 && object3d.Resolutions[SelectedTriangleID] < MAX_RES) {
            object3d.Resolutions[SelectedTriangleID] *= 2;
            object3d.UpdateResolutionsArray(object3d.Resolutions);
            updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        }

        break;

    case Qt::Key_Semicolon:
        DrawingSamplesID = false;
        object3d.UpdateMeshColorsArray(object3d.Points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        break;

    case Qt::Key_Comma:
        DrawingSamplesID = true;
        object3d.UpdateMeshColorsArray(object3d.SelectionPoints);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        break;

    case Qt::Key_Control:
        ControlPressed = true;
        break;

    case Qt::Key_Shift:
        ShiftPressed = true;
        break;

    case Qt::Key_F:
        centerFocus();
        break;
    }

    update();
}

void _gl_widget::mousePressEvent(QMouseEvent *e)
{
    if (e != nullptr && underMouse()) {
        const float x = e->pos().x();
        const float y = height() - e->pos().y();

        if (e->buttons() & Qt::LeftButton && !ControlPressed) {
            selectSample(x, y);
        }
        else if (e->buttons() & Qt::MiddleButton) {
           selectTriangle(x, y);
        }
    }
}

void _gl_widget::mouseMoveEvent(QMouseEvent *e)
{
    if(underMouse()) {
        if(e->buttons() & Qt::LeftButton ) {
            selectSample(e->pos().x(), height() - e->pos().y());
        }

        if(e->buttons() & Qt::RightButton) {
            if(MousePressed) {
               moveCameraRightLeft(QPair<qint32, qint32>(e->pos().x() - PreviousPosition.first, e->pos().y() - PreviousPosition.second));
            }
            else {
                MousePressed = true;
            }

            PreviousPosition.first  = e->pos().x();
            PreviousPosition.second = e->pos().y();
        }
    }
}

void _gl_widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event != nullptr)
    {
        MousePressed = false;
    }
}

void _gl_widget::wheelEvent(QWheelEvent *event)
{
    if(event != nullptr)
    {
        const float value = event->angleDelta().y();

        if(!ControlPressed) {
            addCameraZoom(value);
        }
        else {
            if(value > 0.0f) {
                incrementResolution();
            }
            else {
                decreaseResolution();
            }
        }
    }
}

void _gl_widget::moveCameraRightLeft(QPair<qint32, qint32> InUnits)
{
    if(ControlPressed) {
        Observer_pos_x += 0.005f * InUnits.first;
        Observer_pos_y -= 0.005f * InUnits.second;
    }
    else {
        Observer_angle_y += ANGLE_STEP * InUnits.first;
        Observer_angle_x += ANGLE_STEP * InUnits.second;
    }

    update();
}

void _gl_widget::addCameraZoom(const float InValue)
{
    Observer_distance += (0.005f * (Observer_distance/20.0f)) * InValue;
    update();
}

void _gl_widget::clear_window()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void _gl_widget::change_projection()
{
    camera.reset();

    const float AspectRatio = GLfloat(width()) / GLfloat(height());
    camera.projection.frustum(X_MIN * AspectRatio, X_MAX * AspectRatio, Y_MIN, Y_MAX , FRONT_PLANE_PERSPECTIVE, BACK_PLANE_PERSPECTIVE);
    camera.move(Observer_pos_x, Observer_pos_y, -Observer_distance);
    camera.rotate(Observer_angle_x, 1, 0, 0);
    camera.rotate(Observer_angle_y, 0, 1, 0);
}

void _gl_widget::change_observer()
{
    light.reset();
    light.move(0, 0, -lightDistance);
    light.rotate(lightAngleX, 1, 0, 0);
    light.rotate(lightAngleY, 0, 1, 0);
}

void _gl_widget::drawObject3D()
{
    if (meshColorsShader != nullptr && objectVAO != nullptr) {
        meshColorsShader->bind();
        objectVAO->bind();

        const int matrixLocation = meshColorsShader->uniformLocation("matrix");

        meshColorsShader->setUniformValue(matrixLocation, camera.getProjectedTransform());
        meshColorsShader->setUniformValue("BaseRendering", false);
        meshColorsShader->setUniformValue("LightPos", light.getLocation());
        meshColorsShader->setUniformValue("ColorLerpEnabled", !DrawingSamplesID && ColorLerpEnabled);
        meshColorsShader->setUniformValue("LightingEnabled", !DrawingSamplesID && LightingEnabled);

        glLineWidth(1.5f);

        context->extraFunctions()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glDrawArrays(GL_TRIANGLES, 0, object3d.verticesDrawArrays.size());
        context->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

        objectVAO->release();
        meshColorsShader->release();
    }
}

void _gl_widget::drawAxis()
{
    if (basicRenderingShader != nullptr && axisVAO != nullptr) {
        basicRenderingShader->bind();
        axisVAO->bind();

        glLineWidth(1.0f);

        basicRenderingShader->setUniformValue("matrix", camera.getProjectedTransform());
        basicRenderingShader->setUniformValue("singleLineColor", false);
        basicRenderingShader->setUniformValue("lineColor", QVector4D(0.0f, 0.0f, 0.0f, 0.0f));

        glDrawArrays(GL_LINES, 0, Axis.vertices.size());

        axisVAO->release();
        basicRenderingShader->release();
    }
}

void _gl_widget::drawSelectedTriangle()
{
    if (basicRenderingShader != nullptr && selectedTriangleVAO != nullptr) {
        basicRenderingShader->bind();
        selectedTriangleVAO->bind();

        basicRenderingShader->setUniformValue("matrix", camera.getProjectedTransform());
        basicRenderingShader->setUniformValue("lineColor", QVector4D(0.8f, 0.0f, 0.2f, 0.9f));
        basicRenderingShader->setUniformValue("singleLineColor", true);

        glLineWidth(1.5f);

        glDrawArrays(GL_LINES, 0, SelectedTriangleDrawArray.size());

        selectedTriangleVAO->release();
        basicRenderingShader->release();
    }
}

void _gl_widget::drawObjectWireframe()
{
    if (basicRenderingShader != nullptr && wireframeVAO != nullptr) {
        basicRenderingShader->bind();
        wireframeVAO->bind();

        basicRenderingShader->setUniformValue("matrix", camera.getProjectedTransform());
        basicRenderingShader->setUniformValue("lineColor", QVector4D(0.0f, 0.25f, 1.0f, 0.9f));
        basicRenderingShader->setUniformValue("singleLineColor", true);

        glLineWidth(0.5f);

        glDrawArrays(GL_LINES,0, object3d.trianglesDrawArrays.size());

        wireframeVAO->release();
        basicRenderingShader->release();
    }
}

void _gl_widget::draw_objects()
{
    if(!DrawingSamplesID)
    {
        drawAxis();
    }

    drawObject3D();

    if(SelectedTriangleDrawArray.size() > 0) {
        drawSelectedTriangle();
    }

    if(wireframeMode) {
        drawObjectWireframe();
    }
}

void _gl_widget::drawTrianglesSelectionMode()
{
    clear_window();
    change_projection();
    change_observer();

    basicRenderingShader->bind();
    trianglesIdsVAO->bind();

    basicRenderingShader->setUniformValue("matrix", camera.getProjectedTransform());
    meshColorsShader->setUniformValue("ColorLerpEnabled", false);
    meshColorsShader->setUniformValue("LightingEnabled", false);
    basicRenderingShader->setUniformValue("lineColor", QVector4D(0.0f, 0.3f, 0.8f, 1.0f));
    basicRenderingShader->setUniformValue("singleLineColor", false);

    glDrawArrays(GL_TRIANGLES,0, object3d.verticesDrawArrays.size());

    trianglesIdsVAO->release();
    basicRenderingShader->release();

    basicRenderingShader->bind();

    // Draw axis
    axisVAO->bind();
    glLineWidth(1.0f);

    basicRenderingShader->setUniformValue("matrix", camera.getProjectedTransform());
    basicRenderingShader->setUniformValue("lineColor", QVector4D(0.0f, 0.7f, 1.0f, 1.0f));
    basicRenderingShader->setUniformValue("singleLineColor", false);

    glDrawArrays(GL_LINES, 0, Axis.vertices.size());

    axisVAO->release();
    basicRenderingShader->release();
}

void _gl_widget::paintGL()
{
    clear_window();
    change_projection();
    change_observer();
    draw_objects();
}

void _gl_widget::resizeGL(int Width1, int Height1)
{
    glViewport(0,0,Width1,Height1);
}

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
    object3d = MeshColorsObject3D(vertices, triangles);

    SelectedTriangleDrawArray.clear();
    TriangleSelectionMode = false;

    loadProgram();

    createBuffers();

    logGlInfo();

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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


void _gl_widget::selectSample(const int Selection_position_x, const int Selection_position_y)
{
    if (meshColorsShader == nullptr) return;

    if (!ControlPressed) {
        meshColorsShader->bind();
        meshColorsShader->setUniformValue("ColorLerpEnabled", false);
        meshColorsShader->setUniformValue("LightingEnabled", false);
        meshColorsShader->release();

        QList<int> Indexes;

        DrawingSamplesID = true;

        makeCurrent();

        object3d.UpdateMeshColorsArray(object3d.SelectionPoints);

        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

        update();
        paintGL();

        // get the pixel
        unsigned char data[PencilSize * PencilSize][4];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);

        int PosX = Selection_position_x - (PencilSize * 0.5f);
        int PosY = Selection_position_y - (PencilSize * 0.5f);

        glReadPixels(PosX, PosY, PencilSize, PencilSize, GL_RGBA, GL_UNSIGNED_BYTE, data);

        QMap<int, float> SelectedIDs;

        // Convert the color back to an integer ID
        for(int i = 0; i < PencilSize * PencilSize; i++) {
            const int pickedID =
                    data[i][0] +
                    data[i][1] * 256 +
                    data[i][2] * 256 * 256;

            const float DistanceToCenter = qFabs(0.5f - i/(PencilSize * PencilSize));
            if (data[i][3] == 255 && pickedID != -1 && pickedID < object3d.Points.size()) {
                if (!SelectedIDs.contains(pickedID)) {
                    SelectedIDs.insert(pickedID, DistanceToCenter);
                    object3d.Points[pickedID] =  (1.0f - PencilTransparency) * object3d.Points[pickedID] + PencilTransparency * QVector4D(CurrentPaintingColor.red()/255.0f, CurrentPaintingColor.green()/255.0f, CurrentPaintingColor.blue()/255.0f, 1.0f);

                    if (!Indexes.contains(pickedID)) {
                        Indexes.append(i);
                    }
                }
            }
        }

        object3d.UpdateMeshColorsArray(object3d.Points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        DrawingSamplesID = false;

        meshColorsShader->bind();
        meshColorsShader->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
        meshColorsShader->setUniformValue("LightingEnabled", true);
        meshColorsShader->release();
    }
    else {
        selectTriangle(Selection_position_x, Selection_position_y);
    }
}

void _gl_widget::selectTriangle(const int Selection_position_x, const int Selection_position_y)
{
    if (meshColorsShader == nullptr) return;

    meshColorsShader->bind();
    meshColorsShader->setUniformValue("ColorLerpEnabled", false);
    meshColorsShader->setUniformValue("LightingEnabled", false);
    meshColorsShader->release();

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

    if(data[3] == 255 && pickedID >= 0 && pickedID < object3d.triangles.size()) {
        SelectedTriangleID = pickedID;
        SelectedTriangle = object3d.triangles[SelectedTriangleID];

        selectedTriangleVAO = new QOpenGLVertexArrayObject();
        selectedTriangleVAO->create();
        selectedTriangleVAO->bind();

        SelectedTriangleDrawArray.clear();

        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.y()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.x()]);
        SelectedTriangleDrawArray.push_back(object3d.vertices[SelectedTriangle.z()]);

        initializeBuffer(basicRenderingShader, SelectedTriangleDrawArray.data(), SelectedTriangleDrawArray.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
        initializeBuffer(basicRenderingShader, new QVector4D(1.0f, 0.0f, 0.0f, 1.0f), sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

        selectedTriangleVAO->release();
    }

    TriangleSelectionMode = false;

    meshColorsShader->bind();
    meshColorsShader->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);
    meshColorsShader->setUniformValue("LightingEnabled", true);
    meshColorsShader->release();

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
    if (QOpenGLFunctions* functions = context ? context->functions() : nullptr) {
        functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, InSsbo);
        functions->glBufferData(GL_SHADER_STORAGE_BUFFER, InSize, InData, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
        functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }
}

void _gl_widget::setObjectPath(const char* InNewPath)
{
    ModelFilePath = InNewPath;

    initializeGL();

    centerFocus();

    update();
}

void _gl_widget::setMeshColorsArray(QVector<QVector4D> InColors)
{
    object3d.Points = InColors;
    object3d.UpdateMeshColorsArray(InColors);
    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

void _gl_widget::setResolutionsArray(QVector<int> InRes)
{
    object3d.Resolutions = InRes;
    object3d.UpdateResolutionsArray(InRes);
    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
}

void _gl_widget::centerFocus()
{
    Observer_pos_x = 0.0f;
    Observer_pos_y = 0.0f;
    Observer_angle_y = 0.0f;
    Observer_angle_x = 0.0f;
    Observer_distance = 10.0f;
}

const QVector<QVector4D>& _gl_widget::getMeshColorsArray() const
{
    return object3d.Points;
}

const QVector<int>& _gl_widget::getResolutionsArray() const
{
    return object3d.Resolutions;
}

const _object3D* _gl_widget::getObject3D() const
{
    return &object3d;
}

void _gl_widget::forceUpdate()
{
    updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
    update();
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
    if(SelectedTriangleID >= 0 && SelectedTriangleID < object3d.Resolutions.size() && object3d.Resolutions[SelectedTriangleID] < MAX_RES)
    {
        object3d.Resolutions[SelectedTriangleID] *= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        object3d.UpdateMeshColorsArray(object3d.Points);
        updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);
        update();
    }
}

void _gl_widget::decreaseResolution()
{
    if(SelectedTriangleID >= 0 && SelectedTriangleID < object3d.Resolutions.size() && SelectedTriangleID >= 0 && object3d.Resolutions[SelectedTriangleID] > 2)
    {
        object3d.Resolutions[SelectedTriangleID] /= 2;
        object3d.UpdateResolutionsArray(object3d.Resolutions);
        object3d.UpdateMeshColorsArray(object3d.Points);
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
    meshColorsShader = new QOpenGLShaderProgram(context);

    const QString& path = QString(PROJECT_PATH) + "src/shaders/";

    meshColorsShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   path + "MeshColorsVertex.vsh");
    meshColorsShader->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, path + "MeshColorsGeometry.gsh");
    meshColorsShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, path + "MeshColorsFragment.fsh");
    meshColorsShader->link();

    basicRenderingShader = new QOpenGLShaderProgram(context);

    basicRenderingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   path + "BaseVertex.vsh");
    basicRenderingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, path + "BaseFragment.fsh");
    basicRenderingShader->link();
}

void _gl_widget::createBuffers()
{
    if(context != nullptr) {
        if(meshColorsShader != nullptr) {
            meshColorsShader->bind();

            objectVAO = new QOpenGLVertexArrayObject();
            if (objectVAO->create()) {
                objectVAO->bind();

                initializeBuffer(meshColorsShader, object3d.verticesDrawArrays.data(), object3d.verticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
                initializeBuffer(meshColorsShader, object3d.VerticesNormals.data(), object3d.VerticesNormals.size() * sizeof(QVector3D), "normals", GL_FLOAT, 0, 3);

                meshColorsShader->setUniformValue("ColorLerpEnabled", ColorLerpEnabled);

                context->functions()->glGenBuffers(1, &ssbo);

                updateSSBO(ssbo, sizeof(*object3d.ssbo), object3d.ssbo);

                objectVAO->release();
            }

            meshColorsShader->release();
        }

        if(basicRenderingShader != nullptr) {
            basicRenderingShader->bind();

            trianglesIdsVAO = new QOpenGLVertexArrayObject();
            if (trianglesIdsVAO->create()) {
                trianglesIdsVAO->bind();

                initializeBuffer(basicRenderingShader, object3d.verticesDrawArrays.data(), object3d.verticesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
                initializeBuffer(basicRenderingShader, object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

                trianglesIdsVAO->release();
            }

            axisVAO = new QOpenGLVertexArrayObject();
            if (axisVAO->create()) {
                axisVAO->bind();

                initializeBuffer(basicRenderingShader, Axis.vertices.data(), Axis.vertices.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
                initializeBuffer(basicRenderingShader, Axis.colors.data(), Axis.colors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

                axisVAO->release();
            }

            wireframeVAO = new QOpenGLVertexArrayObject();
            if (wireframeVAO->create()) {
                wireframeVAO->bind();

                initializeBuffer(basicRenderingShader, object3d.trianglesDrawArrays.data(), object3d.trianglesDrawArrays.size() * sizeof(QVector3D),"vertex", GL_FLOAT, 0, 3);
                initializeBuffer(basicRenderingShader, object3d.TriangleSelectionColors.data(), object3d.TriangleSelectionColors.size() * sizeof(QVector4D), "color", GL_FLOAT, 0, 4);

                wireframeVAO->release();
            }

            basicRenderingShader->release();
        }
    }
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

