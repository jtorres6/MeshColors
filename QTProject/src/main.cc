/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include <QApplication>
#include <QMainWindow>
#include <QSurfaceFormat>
#include "window.h"
#include "loadingwindow.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int main( int argc, char ** argv )
{
    QApplication Application( argc, argv );

    // Force the style to be the same on all OSs:
    Application.setStyle("Fusion");

    // Now use a palette to switch to dark colors:
    QPalette palette = QPalette();
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    Application.setPalette(palette);

    QSurfaceFormat Format;
    Format.setDepthBufferSize(24);
    Format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(Format);

    LoadingWindow loadingWindow;
    loadingWindow.show();

    _window Window;
    Window.show();
    loadingWindow.close();
    return Application.exec();
}
