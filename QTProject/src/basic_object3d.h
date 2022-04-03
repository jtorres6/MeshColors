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
#ifdef PLATFORM_WINDOWS
#include "windows.h"
#endif
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
  QVector<QVector3D> vertices;
  QVector<QVector4D> colors;
};

#endif
