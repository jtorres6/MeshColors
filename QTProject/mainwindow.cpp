#include "mainwindow.h"

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

    for(int i = 0; i<Positions.size(); i++)
    {
        VertexID.push_back(QVector3D(i,0,0));
    }
    int pixel[4] = {0,0,0,0};
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeGL()
{
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
    glViewport(0,0, width(), height());
    glClearColor(0.2, 0.3, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset Model View Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3D Transformation
    glTranslatef(0.0, 0.0, -distance);
    glRotatef(rotationX,1,0,0);
    glRotatef(rotationY,0,1,0);

    glDisable(GL_LIGHTING);

    glPointSize(5.0);
    glBegin(GL_POINTS);
    for(unsigned int i=0; i<=Coordinates.size()-3; i+=3)
    {
        glColor3i(VertexID[i][0], VertexID[i][1], VertexID[i][2]);
        glVertex3f(Coordinates[i], Coordinates[i+1], Coordinates[i+2]);
    }
    glEnd();

    glPolygonMode(GL_FRONT,GL_FILL);
    glBegin(GL_TRIANGLES);
    for(unsigned int i=0; i< Positions.size(); i+=1)
    {

       glColor3i(VertexID[i][0], VertexID[i][1], VertexID[i][2]);
       glVertex3f(Coordinates[Positions[i]*3], Coordinates[Positions[i]*3+1], Coordinates[Positions[i]*3+2]);
    }
    glEnd();

    glColor3i(100, 2147483647 , 1147483647 );
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT,GL_LINE);
    glBegin(GL_TRIANGLES);
    for(unsigned int i=0; i< Positions.size(); i+=1)
    {
        glVertex3f(Coordinates[Positions[i]*3], Coordinates[Positions[i]*3+1], Coordinates[Positions[i]*3+2]);
    }
    glEnd();

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

    glReadPixels(event->pos().x(), height()-event->pos().y(), 1, 1, GL_RGB, GL_INT, &pixel);
    qDebug() << pixel[0] << pixel[1] << pixel[2];
}

