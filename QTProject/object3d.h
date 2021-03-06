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


typedef struct ssbo_data
{
    int Resolution[9000];
    QVector4D Colors[9000][64];
}ssbo_type;


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

    int SsboSize();

    void UpdateMeshColorsArray(QVector<QVector4D> p);
    void UpdateResolutionsArray(const QVector<int>& InNewResolutions);

    bool TriangleSelectionMode = true;

    QVector<QVector4D> points, selectionPoints;

    QVector<ssbo_data> MeshColorArray;
    QVector<ssbo_data> MeshSelectionArray;
    QVector<QVector4D> ColorArray;

    ssbo_data* ssbo;

    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

};

#endif // OBJECT3D_H
