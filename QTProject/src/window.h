/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardPaths>
#include <QFileDialog>
#include <QDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGuiApplication>


class _gl_widget;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _window : public QMainWindow
{
    Q_OBJECT

public:
    _window();

public slots:
    void OpenFileDialog();
    void OpenColorDialog();
    void SaveImage();
    void SaveImageAs();
    void LoadMeshColorsFile();

private:

  _gl_widget *m_glWidget;
  QWidget *Options_widget;
  QDialog *helpDialog;

  QPushButton *Color_selection;
  QString SaveFileName;
};

#endif
