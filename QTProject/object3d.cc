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
        vertices.push_back(QVector3D(Coordinates[i],Coordinates[i+1],Coordinates[i+2]));
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
        selectionPoints.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, 1.0f));
    }

    for (int i= 0; i < Triangles.length(); i++)
    {
        Resolutions.push_back(4);

        VerticesDrawArrays[i*3]   = vertices[Triangles[i].z()];
        VerticesDrawArrays[i*3+1] = vertices[Triangles[i].y()];
        VerticesDrawArrays[i*3+2] = vertices[Triangles[i].x()];

        TrianglesDrawArrays.push_back(vertices[Triangles[i].z()]);
        TrianglesDrawArrays.push_back(vertices[Triangles[i].y()]);
        TrianglesDrawArrays.push_back(vertices[Triangles[i].y()]);
        TrianglesDrawArrays.push_back(vertices[Triangles[i].x()]);
        TrianglesDrawArrays.push_back(vertices[Triangles[i].x()]);
        TrianglesDrawArrays.push_back(vertices[Triangles[i].z()]);

        // Convert "i", the integer mesh ID, into an RGB color
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;

        QVector4D TriangleID = QVector4D(r/255.0f, g/255.0f, b/255.0f, 1.0f);
        TriangleSelectionColors.push_back(TriangleID);
        TriangleSelectionColors.push_back(TriangleID);
        TriangleSelectionColors.push_back(TriangleID);

        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));

        const QVector3D& CrossProduct = QVector3D::crossProduct(vertices[int(Triangles[i].y())] - vertices[int(Triangles[i].z())], vertices[int(Triangles[i].x())] - vertices[int(Triangles[i].z())]);

        QVector4D Normal = QVector4D(CrossProduct.normalized(),1.0f);

        VerticesNormals.push_back(Normal);
        VerticesNormals.push_back(Normal);
        VerticesNormals.push_back(Normal);
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
    QMap<QPair<int,int>, int> EdgeIndexMap;
    face_data TriangleData;

    int index = vertices.size();

    for(size_t i = 0; i < Triangles.size(); i++)
    {
        // Colors per face:
        int faceindex = index;
        int R = 4;

        if(ssbo != nullptr && ssbo->Resolution[i] <= 64)
        {
            R = ssbo->Resolution[i];
        }

        index += (R - 1) * (R - 2);

        QPair<int, int> Edges[3];
        Edges[0] = qMakePair(Triangles[i].z(),Triangles[i].y());
        Edges[1] = qMakePair(Triangles[i].y(),Triangles[i].x());
        Edges[2] = qMakePair(Triangles[i].x(),Triangles[i].z());

        for(size_t i = 0; i < 3; i++)
        {
            int edgeindex = index;

            QPair<int, int> invertedPair = qMakePair(Edges[i].second, Edges[i].first);

            const bool IsInverted = EdgeIndexMap.contains(invertedPair);

            if(IsInverted)
            {
                edgeindex = *EdgeIndexMap.find(invertedPair);
            }
            else
            {
                EdgeIndexMap.insert(Edges[i], edgeindex);
            }

            TriangleData.EdgeInfo[i].first = edgeindex;
            TriangleData.EdgeInfo[i].second = IsInverted;

            index += R-1;
        }

        index = faceindex + MAX_SAMPLES;

        TriangleData.FaceIndex = faceindex;

        int Res = ssbo->Resolution[i];

        ssbo->Colors[i][Res * (Res + 1) + 0] = InSamples[int(Triangles[i].x())];
        ssbo->Colors[i][Res]                 = InSamples[int(Triangles[i].y())];
        ssbo->Colors[i][0]                   = InSamples[int(Triangles[i].z())];

        int edgeIndexOffset = 0;
        int faceIndexOffset = 0;

        const int ColorsPerEdge = ((Resolutions[i] - 1) * (Resolutions[i] - 2))/2 - 1;

        // Cij => C0k, Ck0, Ck(R-k) => 0 < k < R
        for(int a = 1; a < Res; a++)
        {
            ssbo->Colors[i][0 * (Res+1) + a]       = InSamples[int(TriangleData.EdgeInfo[0].first + (TriangleData.EdgeInfo[0].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset))];
            ssbo->Colors[i][a * (Res+1) + (Res-a)] = InSamples[int(TriangleData.EdgeInfo[1].first + (TriangleData.EdgeInfo[1].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset))];
            ssbo->Colors[i][a * (Res+1) + 0]       = InSamples[int(TriangleData.EdgeInfo[2].first + (!TriangleData.EdgeInfo[2].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset))];
            edgeIndexOffset++;
        }

        // 0 < a < R, 0 < j < R, a + j != R
        for(int a = 1; a < Res; a++) {
            for(int j = 1; j < Res; j++) {
                if(!(a + j == Res)) {
                    ssbo->Colors[i][a * (Res + 1) + j] = InSamples[int(TriangleData.FaceIndex + faceIndexOffset)];
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
