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

class _object3D : public _basic_object3D
{
public:
    QVector<QVector3D> triangles;
    QVector<QVector3D> verticesDrawArrays;
    QVector<QVector3D> trianglesDrawArrays;
    QVector<QVector4D> TriangleSelectionColors;
    QVector<QVector3D> VerticesNormals;

    _object3D();
    _object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles);

    void ReadPlyFile(const char *Filename);

private:
    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

};

#endif // OBJECT3D_H
