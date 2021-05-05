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

class _object3D:public _basic_object3D
{
    public:
    QVector<QVector3D> Triangles;
    QVector<QVector3D> VerticesDrawArrays;
    QVector<QVector3D> MeshColorsData;
    QVector<QVector3D> Index;
    QVector<QVector4D> PerFaceData;
    QVector<QVector4D> TriangleSelectionColors;

    QVector<int> Resolutions;

    QMap<QPair<int,int>, int> EdgeIndexMap;

    _object3D();
    _object3D(const char *Filename);

    void ReadPlyFile(const char *Filename);

    bool TriangleSelectionMode = true;
};

#endif // OBJECT3D_H
