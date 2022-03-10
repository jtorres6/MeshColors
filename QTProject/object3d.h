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
const int MAX_SAMPLES = 2150;

typedef struct ssbo_data
{
    int Resolution[MAX_TRIANGLES];
    QVector4D Colors[MAX_TRIANGLES][MAX_SAMPLES];
}ssbo_type;

struct face_data
{
    int FaceIndex;
    QPair<int, bool> EdgeInfo[3];

    face_data(int InFaceIndex, QPair<int, bool> (InEdgeInfo)[3])
    {
        FaceIndex = InFaceIndex;
        EdgeInfo[0] = InEdgeInfo[0];
        EdgeInfo[1] = InEdgeInfo[1];
        EdgeInfo[2] = InEdgeInfo[2];
    }

    face_data()
    {
        FaceIndex = -1;
        EdgeInfo[0] = QPair<int, bool>(0,false);
        EdgeInfo[1] = QPair<int, bool>(0,false);
        EdgeInfo[2] = QPair<int, bool>(0,false);
    }
};

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

    int SsboSize();

    void UpdateMeshColorsArray(const QVector<QVector4D>& InSamples);
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
