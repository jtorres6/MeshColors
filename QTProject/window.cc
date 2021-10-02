/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QFrame>
#include <QWheelEvent>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QSpinBox>
#include <QGLFormat>
#include <QSlider>
#include <debugtools.h>

#include "window.h"
#include "glwidget.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_window::_window()
{
    QSizePolicy Q(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QWidget *Central_widget = new QWidget(this);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::black);

    QWidget *Options_widget = new QWidget;
    QVBoxLayout *Vertical_options = new QVBoxLayout;
    QLabel *Label1 = new QLabel("Select color");
    QPushButton *Color_selection = new QPushButton("");
    Color_selection->setAutoFillBackground(true);
    Color_selection->setPalette(Pal);
    connect(Color_selection, SIGNAL(pressed()), this, SLOT(OpenColorDialog()));

    QPushButton *Lighting_button = new QPushButton("Toggle lighting");
    QPushButton *Lerp_button = new QPushButton("Toggle color inerpolation");

    Vertical_options->addWidget(Label1);
    Vertical_options->addWidget(Color_selection);
    Vertical_options->addWidget(Lighting_button);
    Vertical_options->addWidget(Lerp_button);
    Vertical_options->addStretch();

    QLabel *Label2 = new QLabel("Face resolution");
    QPushButton *FaceSelection_button = new QPushButton("Enable face selection");
    QPushButton *Increment_button = new QPushButton("+");
    QPushButton *Decrease_button = new QPushButton("-");
    Vertical_options->addWidget(Label2);
    Vertical_options->addWidget(FaceSelection_button);
    Vertical_options->addWidget(Increment_button);
    Vertical_options->addWidget(Decrease_button);
    Vertical_options->addStretch();

    QLabel *Label3 = new QLabel("Pencil size");
    QSlider* PencilSize_widget = new QSlider(Qt::Horizontal);
    PencilSize_widget->setMinimum(1);
    PencilSize_widget->setMaximum(32);
    Vertical_options->addWidget(Label3);
    Vertical_options->addWidget(PencilSize_widget);

    Options_widget->setLayout(Vertical_options);

    QTabWidget *Tab_widget = new QTabWidget;
    Tab_widget->setMaximumWidth(300);
    Tab_widget->addTab(Options_widget, "Options");

    QFrame *Framed_widget= new QFrame(Central_widget);
    Framed_widget->setSizePolicy(Q);
    Framed_widget->setFrameStyle(QFrame::Panel);
    Framed_widget->setLineWidth(3);

    GL_widget = new _gl_widget(this);
    GL_widget->setSizePolicy(Q);

    connect(FaceSelection_button, SIGNAL(pressed()), GL_widget, SLOT(EnableTriangleSelectionMode()));
    connect(Increment_button, SIGNAL(pressed()), GL_widget, SLOT(IncrementResolution()));
    connect(Decrease_button, SIGNAL(pressed()), GL_widget, SLOT(DecreaseResolution()));
    connect(Lighting_button, SIGNAL(pressed()), GL_widget, SLOT(ToggleLighting()));
    connect(Lerp_button, SIGNAL(pressed()), GL_widget, SLOT(ToggleColorInterpolation()));
    connect(PencilSize_widget, SIGNAL(valueChanged(int)), GL_widget, SLOT(UpdatePencilSize(int)));

    QHBoxLayout *Horizontal_frame = new QHBoxLayout();
    Horizontal_frame->setContentsMargins(1,1,1,1);

    Horizontal_frame->addWidget(GL_widget);
    Horizontal_frame->addWidget(Tab_widget);

    Framed_widget->setLayout(Horizontal_frame);

    QHBoxLayout *Horizontal_main = new QHBoxLayout(Central_widget);

    Horizontal_main->addWidget(Framed_widget);

    Central_widget->setLayout(Horizontal_main);
    setCentralWidget(Central_widget);

    // actions for file menu
    QAction *Exit = new QAction(QIcon("./icons/exit.png"), tr("&Exit..."), this);
    Exit->setShortcut(tr("Ctrl+Q"));
    Exit->setToolTip(tr("Exit the application"));
    connect(Exit, SIGNAL(triggered()), this, SLOT(close()));

    // actions for file menu
    QAction *FileOpen = new QAction(tr("&Open file..."), this);
    FileOpen->setShortcut(tr("Ctrl+E"));
    connect(FileOpen, SIGNAL(triggered()), this, SLOT(OpenFileDialog()));

    // actions for file menu
    QAction *SaveTexture = new QAction(tr("&Save image"), this);
    SaveTexture->setShortcut(tr("Ctrl+S"));
    connect(SaveTexture, SIGNAL(triggered()), this, SLOT(close()));

    QAction *SaveTextureAs = new QAction(tr("&Save image as..."), this);
    SaveTextureAs->setShortcut(tr("Ctrl+Shift+S"));
    connect(SaveTextureAs, SIGNAL(triggered()), this, SLOT(SaveImage()));

    // actions for file menu
    QAction *LoadTexture = new QAction(tr("Load image"), this);
    connect(LoadTexture, SIGNAL(triggered()), this, SLOT(LoadMeshColorsFile()));

    // menus
    QMenu *File_menu=menuBar()->addMenu(tr("&File"));
    File_menu->addAction(SaveTexture);
    File_menu->addAction(SaveTextureAs);
    File_menu->addAction(LoadTexture);
    File_menu->addAction(FileOpen);
    File_menu->addAction(Exit);
    File_menu->setAttribute(Qt::WA_AlwaysShowToolTips);

    setWindowTitle(tr("MeshColors"));

    resize(800,800);
}

void _window::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        GL_widget->pick(e->pos().x(), height() - e->pos().y());
    }
}

void _window::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        GL_widget->pick(e->pos().x(), height() - e->pos().y());
    }

    if(e->buttons() & Qt::RightButton)
    {
        if(MousePressed)
        {
            GL_widget->MoveCameraRightLeft(QPair<qint32, qint32>(e->pos().x() - PreviousPosition.first, e->pos().y() - PreviousPosition.second));
        }
        else
        {
            MousePressed = true;
        }

        PreviousPosition.first  = e->pos().x();
        PreviousPosition.second = e->pos().y();
    }
}

void _window::mouseReleaseEvent(QMouseEvent *event)
{
    if(event != nullptr)
    {
        MousePressed = false;
    }
}

void _window::wheelEvent(QWheelEvent *event)
{
    if(event != nullptr)
    {
        GL_widget->AddCameraZoom(event->angleDelta().y());
    }
}

void _window::resizeEvent(QResizeEvent *event)
{
    if(event != nullptr)
    {
        GL_widget->resize(event->size().width(), event->size().height());
    }
}

void _window::OpenFileDialog()
{
    // Posible fix: https://blogs.kde.org/2009/03/26/how-crash-almost-every-qtkde-application-and-how-fix-it-0
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("3D object location"), "",
        tr("PLY objects (*.ply);;All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

    QDir dir(QDir::currentPath());

    std::string FileNameStd = dir.relativeFilePath(fileName).toStdString();

    const char *fileNameChar = FileNameStd.c_str();
    GL_widget->SetObjectPath(fileNameChar);
}

void _window::SaveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Mesh Colors file location"), "",
        tr("Mesh colors map (*.mcm);;All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);
    out << GL_widget->GetResolutionsArray();
    out << GL_widget->GetMeshColorsArray();
}

void _window::LoadMeshColorsFile()
{
    // Posible fix: https://blogs.kde.org/2009/03/26/how-crash-almost-every-qtkde-application-and-how-fix-it-0
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Mesh Colors file location"), "",
        tr("Mesh colors map (*.mcm);;All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);
    QVector<int> resolutions;
    QVector<QVector4D> array;
    in >> resolutions;
    in >> array;
    GL_widget->SetResolutionsArray(resolutions);
    GL_widget->SetMeshColorsArray(array);
}

void _window::OpenColorDialog()
{
     QColor NewColor = QColorDialog::getColor(Qt::yellow, this );
     GL_widget->SetCurrentPaintingColor(NewColor);
}
