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

    Vertical_options->addWidget(Label1);
    Vertical_options->addWidget(Color_selection);
    Vertical_options->addStretch();

    QLabel *Label2 = new QLabel("Face resolution");
    QPushButton *Increment_button = new QPushButton("+");
    QPushButton *Decrease_button = new QPushButton("-");
    Vertical_options->addWidget(Label2);
    Vertical_options->addWidget(Increment_button);
    Vertical_options->addWidget(Decrease_button);
    Vertical_options->addStretch();

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
    QAction *FileOpen = new QAction(QIcon("./icons/exit.png"), tr("&Open file..."), this);
    FileOpen->setShortcut(tr("Ctrl+E"));
    FileOpen->setToolTip(tr("Exit the application"));
    connect(FileOpen, SIGNAL(triggered()), this, SLOT(OpenFileDialog()));

    // menus
    QMenu *File_menu=menuBar()->addMenu(tr("&File"));
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
    MousePressed = false;
}

void _window::wheelEvent(QWheelEvent *event)
{
    GL_widget->AddCameraZoom(event->delta());
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

    const char *fileNameChar = FileNameStd.c_str();//dir.relativeFilePath(fileName).toUtf8().data();
    GL_widget->SetObjectPath(fileNameChar);
}

void _window::OpenColorDialog()
{
     QColor NewColor = QColorDialog::getColor(Qt::yellow, this );
     GL_widget->SetCurrentPaintingColor(NewColor);
}
