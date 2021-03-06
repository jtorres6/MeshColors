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

  QFrame *Framed_widget= new QFrame(Central_widget);
  Framed_widget->setSizePolicy(Q);
  Framed_widget->setFrameStyle(QFrame::Panel);
  Framed_widget->setLineWidth(3);


  GL_widget = new _gl_widget(this);
  GL_widget->setSizePolicy(Q);

  QHBoxLayout *Horizontal_frame = new QHBoxLayout();
  Horizontal_frame->setContentsMargins(1,1,1,1);

  Horizontal_frame->addWidget(GL_widget);
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

  // menus
  QMenu *File_menu=menuBar()->addMenu(tr("&File"));
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
