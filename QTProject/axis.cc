/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "axis.h"

using namespace _colors_ne;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_axis::_axis(float Size)
{
    Vertices.resize(6);
    Vertices[0] = QVector3D(Size, 0.0, 0.0);// x
    Vertices[1] = QVector3D(-Size, 0.0, 0.0);
    Vertices[2] = QVector3D(0.0, Size, 0.0); // y
    Vertices[3] = QVector3D(0.0, -Size, 0.0);
    Vertices[4] = QVector3D(0.0, 0.0, Size); // z
    Vertices[5] = QVector3D(0.0, 0.0, -Size);

    Colors.resize(6);
    Colors[0] = QVector3D(1, 0, 0); // red
    Colors[1] = QVector3D(1, 0, 0);
    Colors[2] = QVector3D(0, 1, 0); // green
    Colors[3] = QVector3D(0, 1, 0);
    Colors[4] = QVector3D(0, 0, 1); // blue
    Colors[5] = QVector3D(0, 0, 1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/


void _axis::draw_line()
{
}



