/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "object3d.h"
#include "file_ply_stl.h"
#include <QDebug>
#include <map>

using namespace _colors_ne;

void _object3D::ReadPlyFile(const char *Filename)
{
    _file_ply ply;
    if(ply.open(Filename) == 0) return;

    vector<float> Coordinates;
    vector<unsigned int> Positions;
    ply.read(Coordinates, Positions);

    for(size_t i = 0; i <= Coordinates.size()-3; i+=3)
    {
        Vertices.push_back(QVector3D(Coordinates[i],Coordinates[i+1],Coordinates[i+2]));
    }

    for(size_t i = 0; i <= Positions.size()-3; i+=3)
    {
        Triangles.push_back(QVector3D(Positions[i],Positions[i+1],Positions[i+2]));
    }
}

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

_object3D::_object3D()
{
}

_object3D::_object3D(const char *Filename)
{
    ReadPlyFile(Filename);

    ssbo = (ssbo_data*)malloc(sizeof(ssbo_data));

    VerticesDrawArrays.resize(Triangles.size()*3);

    for(size_t i = 0; i < MAX_TRIANGLES*MAX_SAMPLES; i++)
    {
        if(i%2 == 0)
        {
            points.push_back(QVector4D(0.2f, 0.2f, 0.2f, 1.0f));
        }
        else
        {
            points.push_back(QVector4D(0.4f, 0.4f, 0.4f, 1.0f));
        }

        // Convert "i", the integer mesh ID, into an RGB color
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;

        // TODO: This should be r/255.0f, but it only works with 254 somehow. CHECK THIS!
        selectionPoints.push_back(QVector4D(r/254.0f, g/254.0f, b/254.0f, 1.0f));
    }

    for (int i= 0; i < Triangles.length(); i++)
    {
        Resolutions.push_back(4);

        VerticesDrawArrays[i*3]=Vertices[Triangles[i].x()];
        VerticesDrawArrays[i*3+1]=Vertices[Triangles[i].y()];
        VerticesDrawArrays[i*3+2]=Vertices[Triangles[i].z()];

        Colors.push_back(QVector4D(1.0, 0.0, 0.0, 1.0));
        Colors.push_back(QVector4D(0.0, 1.0, 0.0, 1.0));
        Colors.push_back(QVector4D(0.0, 0.0, 1.0, 1.0));

        // Convert "i", the integer mesh ID, into an RGB color
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;

        TriangleSelectionColors.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, 1.0f));
        TriangleSelectionColors.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, 1.0f));
        TriangleSelectionColors.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, 1.0f));

        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));
    }

    UpdateResolutionsArray(Resolutions);
    UpdateMeshColorsArray(points);
}

int _object3D::SsboSize()
{
    return sizeof(ssbo_data);
}

void _object3D::UpdateMeshColorsArray(const QVector<QVector4D>& InSamples)
{
    int index = Vertices.size() - 1;
    QPair<int, bool> EdgeInfo[3];
    EdgeIndexMap.clear();
    PerFaceData.clear();

    for(size_t i = 0; i < Triangles.size(); i++)
    {
        // Colors per face:
        int faceindex = index;
        int R = 4;

        if(ssbo != nullptr && ssbo->Resolution[i] <= 64)
        {
            R = ssbo->Resolution[i];
        }
        index += ((R - 1) * (R - 2))/2;
        int edge1index = index;
        bool IsInverted = false;
        QPair<int, int> pair = qMakePair(Triangles[i].x(),Triangles[i].y());

        EdgeIndexMap.insert(pair, edge1index);
        EdgeInfo[0].first = edge1index;
        EdgeInfo[0].second = IsInverted;

        index += R-1;
        int edge2index = index;
        pair =  qMakePair(Triangles[i].y(),Triangles[i].z());

        EdgeIndexMap.insert(pair, edge2index);
        EdgeInfo[1].first = edge2index;
        EdgeInfo[1].second = IsInverted;

        // Colors per edge (R-1)+1:
        index += R-1;
        int edge3index = index;

        pair =  qMakePair(Triangles[i].z(),Triangles[i].x());

        EdgeIndexMap.insert(pair, edge3index);
        EdgeInfo[2].first = edge3index;
        EdgeInfo[2].second = IsInverted;

        index = faceindex + MAX_SAMPLES;

        qDebug() << i << faceindex << EdgeInfo[0].first << EdgeInfo[1].first << EdgeInfo[2].first;

        PerFaceData.push_back(face_data(faceindex, EdgeInfo));
    }

    for(int i = 0; i < Triangles.size(); i++)
    {
        int Res = ssbo->Resolution[i];

        ssbo->Colors[i][Res * (Res + 1) + 0] = InSamples[int(Triangles[i].x())];
        ssbo->Colors[i][Res]                 = InSamples[int(Triangles[i].y())];
        ssbo->Colors[i][0]                   = InSamples[int(Triangles[i].z())];

        int faceIndexOffset = 0;
        int edgeIndexOffset = 0;

        // Cij => C0k, Ck0, Ck(R-k) => 0 < k < R
        for(int a = 1; a < Res; a++)
        {
            ssbo->Colors[i][a * (Res+1) + (Res-a)] = InSamples[int(PerFaceData[i].EdgeInfo[0].first + edgeIndexOffset)];
            ssbo->Colors[i][0 * (Res+1) + a]       = InSamples[int(PerFaceData[i].EdgeInfo[1].first + edgeIndexOffset)];
            ssbo->Colors[i][a * (Res+1) + 0]       = InSamples[int(PerFaceData[i].EdgeInfo[2].first + edgeIndexOffset)];

            edgeIndexOffset++;

            for(int j = 1; j < Res; j++)
            {
                if(!(a + j == Res))
                {
                    ssbo->Colors[i][a * (Res + 1) + j] = InSamples[int(PerFaceData[i].FaceIndex + faceIndexOffset)];
                    faceIndexOffset++;
                }
            }
        }
    }
}

void _object3D::UpdateResolutionsArray(const QVector<int>& InNewResolutions)
{
    for(int i = 0; i < Triangles.size(); i++)
    {
        if(i >= InNewResolutions.size()) return;

        ssbo->Resolution[i] = InNewResolutions[i];
    }
}
