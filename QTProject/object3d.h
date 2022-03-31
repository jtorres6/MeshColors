/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "basic_object3d.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

const int MAX_TRIANGLES = 9000;

class _object3D:public _basic_object3D
{
    public:
    QVector<QVector3D> Triangles;
    QVector<QVector3D> VerticesDrawArrays;
    QVector<QVector3D> MeshColorsData;
    QVector<QVector3D> Index;
    QVector<QVector4D> TriangleSelectionColors;
    QVector<QVector4D> VerticesNormals;
    QVector<QVector3D> TrianglesDrawArrays;

    QVector<int> Resolutions;

    _object3D();
    _object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles);

    void ReadPlyFile(const char *Filename);

    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

};

#endif // OBJECT3D_H
