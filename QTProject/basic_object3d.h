/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef BASIC_OBJECT3D_H
#define BASIC_OBJECT3D_H

#include <vector>
#include "windows.h"
#include <GL/gl.h>
#include "colors.h"
#include "vertex.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _basic_object3D
{
  public:
  QVector<QVector3D> Vertices;
  QVector<QVector3D> Colors;

  QMatrix4x4 Projection;
  QOpenGLShaderProgram* program;

  void draw_point();
};

#endif
