/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */

#include "axis.h"

_axis::_axis()
{
    _axis(5000);
}

_axis::_axis(float Size)
{
    vertices.resize(6);
    vertices[0] = QVector3D(Size, 0.0, 0.0);// x
    vertices[1] = QVector3D(-Size, 0.0, 0.0);
    vertices[2] = QVector3D(0.0, Size, 0.0); // y
    vertices[3] = QVector3D(0.0, -Size, 0.0);
    vertices[4] = QVector3D(0.0, 0.0, Size); // z
    vertices[5] = QVector3D(0.0, 0.0, -Size);

    colors.resize(6);
    colors[0] = QVector3D(1, 0, 0); // red
    colors[1] = QVector3D(1, 0, 0);
    colors[2] = QVector3D(0, 1, 0); // green
    colors[3] = QVector3D(0, 1, 0);
    colors[4] = QVector3D(0, 0, 1); // blue
    colors[5] = QVector3D(0, 0, 1);
}


