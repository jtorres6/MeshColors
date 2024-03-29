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
#include <QCheckBox>
#include <QRadioButton>
#include <QMdiSubWindow>
#include <QDialog>
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

    Options_widget = new QWidget;
    QVBoxLayout *Vertical_options = new QVBoxLayout;
    QLabel *Label1 = new QLabel("Select color");
    Color_selection = new QPushButton("");

    QColor SelectedColor = Qt::black;
    if(SelectedColor.isValid())
    {
        QString qss = QString("background-color: %1").arg(SelectedColor.name());
        Color_selection->setStyleSheet(qss);
    }

    Color_selection->setAutoFillBackground(true);
    Color_selection->setPalette(Pal);
    connect(Color_selection, SIGNAL(pressed()), this, SLOT(OpenColorDialog()));

    QCheckBox *Lighting_button = new QCheckBox("Lighting");
    QCheckBox *Lerp_button = new QCheckBox("Color inerpolation");

    Options_widget->setLayout(Vertical_options);

    Vertical_options->addWidget(Label1);
    Vertical_options->addWidget(Color_selection);

    QLabel *Label3 = new QLabel("Pencil size");
    QSlider* PencilSize_widget = new QSlider(Qt::Horizontal);
    PencilSize_widget->setMinimum(1);
    PencilSize_widget->setSliderPosition(10);
    PencilSize_widget->setMaximum(32);
    Vertical_options->addWidget(Label3);
    Vertical_options->addWidget(PencilSize_widget);

    QLabel *Label4 = new QLabel("Pencil transparency");
    QSlider* PencilTransparency_widget = new QSlider(Qt::Horizontal);
    PencilTransparency_widget->setMinimum(0);
    PencilTransparency_widget->setSliderPosition(50);
    PencilTransparency_widget->setMaximum(100);
    Vertical_options->addWidget(Label4);
    Vertical_options->addWidget(PencilTransparency_widget);
    Vertical_options->addStretch();

    QCheckBox *wireframeEnabledWidget = new QCheckBox("Wireframe", this);

    QLabel *Label2 = new QLabel("Face resolution");
    QPushButton *Increment_button = new QPushButton("+");
    QPushButton *Decrease_button = new QPushButton("-");
    Vertical_options->addWidget(Label2);
    Vertical_options->addWidget(Increment_button);
    Vertical_options->addWidget(Decrease_button);
    Vertical_options->addStretch();

    Vertical_options->addWidget(wireframeEnabledWidget);
    Vertical_options->addWidget(Lighting_button);
    Vertical_options->addWidget(Lerp_button);
    Vertical_options->addStretch();

    QTabWidget *Tab_widget = new QTabWidget;
    Tab_widget->setMaximumWidth(300);
    Tab_widget->addTab(Options_widget, "Options");

    QFrame *Framed_widget= new QFrame(Central_widget);
    Framed_widget->setSizePolicy(Q);
    Framed_widget->setFrameStyle(QFrame::Panel);
    Framed_widget->setLineWidth(1);
    m_glWidget = new _gl_widget();
    m_glWidget->setSizePolicy(Q);
    QHBoxLayout *Test_frame1 = new QHBoxLayout();
    QVBoxLayout *Test_frame2 = new QVBoxLayout();
    Test_frame2->addLayout(Test_frame1);

    helpDialog = new QDialog();
    helpDialog->setStyleSheet("border: solid 10px grey;  background-color: rgba(75, 75, 75, 175); color:rgb(75, 75, 75)");
    QVBoxLayout *vlayout = new QVBoxLayout;
    helpDialog->setFixedSize(300,300);

    QLabel *cameraControl = new QLabel("Camera: \n -Rotate: Left Mouse Button \n -Move: Ctrl + Left Mouse Button \n -Zoom: Mouse wheel");
    cameraControl->setStyleSheet("color:rgb(250,250,250)");

    QLabel *paintControl = new QLabel("Painting and resolution: \n -Paint: Right Mouse Button \n -Select triangle: Ctrl + Right Mouse Button \n -Change face resolution: Ctrl + Mouse wheel");
    paintControl->setStyleSheet("color:rgb(250,250,250)");

    QAbstractButton *bExit = new QPushButton("Hide");

    bExit->setStyleSheet("border: solid 10px grey;  background-color: rgba(255, 255, 255, 255); color:rgb(100,100,100)");
    vlayout->addWidget(cameraControl);
    vlayout->addWidget(paintControl);
    helpDialog->setLayout(vlayout);
    helpDialog->show();
    vlayout->addWidget(bExit);
    helpDialog->connect(bExit,SIGNAL(clicked()),helpDialog,SLOT(close()));

    Test_frame1->addWidget(helpDialog);
    m_glWidget->setLayout(Test_frame2);
    Test_frame1->addStretch();
    Test_frame2->addStretch();

    connect(Increment_button, SIGNAL(pressed()), m_glWidget, SLOT(incrementResolution()));
    connect(Decrease_button, SIGNAL(pressed()), m_glWidget, SLOT(decreaseResolution()));
    connect(Lighting_button, SIGNAL(stateChanged(int)), m_glWidget, SLOT(toggleLighting()));
    connect(Lerp_button, SIGNAL(stateChanged(int)), m_glWidget, SLOT(toggleColorInterpolation()));
    connect(PencilSize_widget, SIGNAL(valueChanged(int)), m_glWidget, SLOT(updatePencilSize(int)));
    connect(PencilTransparency_widget, SIGNAL(valueChanged(int)), m_glWidget, SLOT(updatePencilTransparency(int)));
    connect(wireframeEnabledWidget, SIGNAL(stateChanged(int)), m_glWidget, SLOT(toggleWireframeMode()));

    QHBoxLayout *Horizontal_frame = new QHBoxLayout();
    Horizontal_frame->setContentsMargins(1,1,1,1);

    Horizontal_frame->addWidget(m_glWidget);
    Horizontal_frame->addWidget(Tab_widget);

    Framed_widget->setLayout(Horizontal_frame);

    QHBoxLayout *Horizontal_main = new QHBoxLayout(Central_widget);

    Horizontal_main->addWidget(Framed_widget);

    Central_widget->setLayout(Horizontal_main);
    setCentralWidget(Central_widget);

    // actions for file menu
    QAction *actionExit = new QAction(QIcon("./icons/exit.png"), tr("&Exit..."), this);
    actionExit->setShortcut(tr("Ctrl+Q"));
    actionExit->setToolTip(tr("Exit the application"));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // actions for file menu
    QAction *actionFileOpen = new QAction(tr("&Load PLY model"), this);
    actionFileOpen->setShortcut(tr("Ctrl+E"));
    connect(actionFileOpen, SIGNAL(triggered()), this, SLOT(OpenFileDialog()));

    // actions for file menu
    QAction *actionSaveTexture = new QAction(tr("&Save texture"), this);
    actionSaveTexture->setShortcut(tr("Ctrl+S"));
    connect(actionSaveTexture, SIGNAL(triggered()), this, SLOT(SaveImage()));

    QAction *actionSaveTextureAs = new QAction(tr("&Save texture as..."), this);
    actionSaveTextureAs->setShortcut(tr("Ctrl+Shift+S"));
    connect(actionSaveTextureAs, SIGNAL(triggered()), this, SLOT(SaveImageAs()));

    // actions for file menu
    QAction *actionLoadTexture = new QAction(tr("Load texture"), this);
    connect(actionLoadTexture, SIGNAL(triggered()), this, SLOT(LoadMeshColorsFile()));

    // menus
    QMenu *fileMenu=menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(actionFileOpen);
    fileMenu->addAction(actionSaveTexture);
    fileMenu->addAction(actionSaveTextureAs);
    fileMenu->addAction(actionLoadTexture);
    fileMenu->addAction(actionExit);
    fileMenu->setAttribute(Qt::WA_AlwaysShowToolTips);

    QAction *showControls = new QAction(tr("&Show controls"), this);
    actionSaveTextureAs->setShortcut(tr("Ctrl+Shift+O"));
    connect(showControls, SIGNAL(triggered()), helpDialog, SLOT(show()));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(showControls);

    setWindowTitle(tr("MeshColors"));

    resize(800,800);
}

void _window::OpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("3D object location"), QString(PROJECT_PATH) + "/models/",
        tr("PLY objects (*.ply);;All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

    QDir dir(QDir::currentPath());

    std::string FileNameStd = dir.relativeFilePath(fileName).toStdString();

    const char *fileNameChar = FileNameStd.c_str();
    m_glWidget->setObjectPath(fileNameChar);
}

void _window::SaveImage()
{
    if(!SaveFileName.isEmpty())
    {
        QFile file(SaveFileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_10);

        out << m_glWidget->object3d.Points;
        out << m_glWidget->object3d;
    }
    else
    {
        SaveImageAs();
    }
}

void _window::SaveImageAs()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Chose mesh color texture file"), "",
        tr("Mesh colors map (*.mcm);All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    SaveFileName = fileName;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    out << m_glWidget->object3d.Points;
    out << m_glWidget->object3d;
}

void _window::LoadMeshColorsFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Chose mesh color texture file"), "",
        tr("Mesh colors map (*.mcm);All Files (*)"), 0, QFileDialog::DontUseNativeDialog);

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

    in >> m_glWidget->object3d.Points;
    in >> m_glWidget->object3d;
    m_glWidget->forceUpdate();
}

void _window::OpenColorDialog()
{
     const QColor newColor = QColorDialog::getColor(Qt::yellow, this );

     if(newColor.isValid())
     {
         m_glWidget->setCurrentPaintingColor(newColor);

         QString styleString = QString("background-color: %1").arg(newColor.name());
         Color_selection->setStyleSheet(styleString);
     }
}
