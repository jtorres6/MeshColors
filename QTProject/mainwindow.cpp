#include "mainwindow.h"

float RandomFloat(float a, float b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

MainWindow::MainWindow(QWidget *parent)
{
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(2,1);
    setFormat(format);

    distance = 250;
    rotationX = 0;
    rotationY = 0;

    _file_ply ply;
    ply.open("models/ant.ply");
    ply.read(Coordinates, Positions);

    image.load("images/imagen.png");
    image=image.mirrored(false,true);

    GLfloat pixel[4] = {0.0f,0.0f,0.0f};

    for(unsigned int i=0; i<=Positions.size()/3; i+=1)
    {
        vector<QVector3D> v;
        VertexID.push_back(v);

        int r = rand()%10+1;
        resolutions.push_back(r);

        for(int j =0; j<r*r; j++)
        {
           VertexID[i].push_back(QVector3D(RandomFloat(0.0f,0.9f),RandomFloat(0.0f,0.9f),RandomFloat(0.0f,0.9f)));
        }
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeGL()
{
    program = new QOpenGLShaderProgram(context);
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "MeshColorsVertex.vsh");
    program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"MeshColorsFragment.fsh");
    program->link();
    program->bind();
    glEnable(GL_DEPTH_TEST);
    resizeGL(this->width(), this->height());
}

void MainWindow::resizeGL(int w, int h)
{
    // Set Viewport
    glViewport(0, 0, w, h);
    qreal aspectratio = qreal(w) /qreal(h);

    // Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(75, aspectratio, 0.1, 400000000);

    // Initialize Model View Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void MainWindow::paintGL()
{
    // Remove last render buffer.
    glViewport(0, 0, width(), height());
    glClearColor(0.1, 0.4, 0.4, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset Model View Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3D Transformation
    glTranslatef(0.0, 0.0, -distance);
    glRotatef(rotationX, 1, 0, 0);
    glRotatef(rotationY, 0, 1, 0);

    glDisable(GL_LIGHTING);

    glPointSize(10.5);
    for(unsigned int i=0; i< Positions.size(); i+=3)
    {
        glPolygonMode(GL_FRONT,GL_FILL);
        program->setUniformValue("meshColorsEnabled", true);
        program->setUniformValue("R",resolutions[i/3]);
        program->setUniformValueArray("points",VertexID[i/3].data(),resolutions[i/3]*resolutions[i/3]);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(Coordinates[Positions[i]*3], Coordinates[Positions[i]*3+1], Coordinates[Positions[i]*3+2]);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(Coordinates[Positions[i+1]*3], Coordinates[Positions[i+1]*3+1], Coordinates[Positions[i+1]*3+2]);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(Coordinates[Positions[i+2]*3], Coordinates[Positions[i+2]*3+1], Coordinates[Positions[i+2]*3+2]);
        glEnd();
    }

    glFlush();
    glFinish();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeGL(this->width(), this->height());
    this->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)
    {
        rotationY += 5;
    }
    if(event->key() == Qt::Key_D)
    {
        rotationY -= 5;
    }
    if(event->key() == Qt::Key_W)
    {
        rotationX += 3;
    }
    if(event->key() == Qt::Key_S)
    {
        rotationX -= 3;
    }
    if(event->key() == Qt::Key_F)
    {
        distance += 3;
    }
    if(event->key() == Qt::Key_R)
    {
        distance -= 3;
    }
    if(event->key() == Qt::Key_P)
    {
        if(resolution < 9)
        {
            resolutionUpdated = false;
            resolution += 1;
        }
    }
    if(event->key() == Qt::Key_O)
    {
        if(resolution > 1)
        {
            resolutionUpdated = false;
            resolution -= 1;
        }
    }

    this->update();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
     QPoint points = event->angleDelta() / 8;
     distance += points.y();
     this->update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    glFlush();
    glFinish();

    glReadPixels(event->pos().x(), height()-event->pos().y(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    //qDebug() << pixel[0] << pixel[1] << pixel[2];
    this->update();
}

