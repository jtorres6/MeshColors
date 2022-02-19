/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "object3d.h"
#include "file_ply_stl.h"
#include <map>
#include <QDebug>

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

float RandomFloat(const float a, const float b) {
    const float random = ((float) rand()) / (float) RAND_MAX;
    const float diff = b - a;
    const float r = random * diff;
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
        selectionPoints.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, i/float(MAX_TRIANGLES*MAX_SAMPLES)));
    }

    for (int i= 0; i < Triangles.length(); i++)
    {
        Resolutions.push_back(1);

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

void _object3D::UpdateMeshColorsArray(QVector<QVector4D>& InSamples)
{
    if(ssbo == nullptr)
        return;

    QMap<QPair<int,int>, int> EdgeIndexMap;
    face_data TriangleData;

    int index = vertices.size();

    for(int i = 0; i < Triangles.size(); i++)
    {
        // Colors per face:
        int faceindex = index;
        int R = 4;

        if(ssbo->Resolution[i] <= 64)
        {
            R = ssbo->Resolution[i];

            if(i >= Faces.size())
            {
                Faces.push_back(meshColorsFace(R));
            }
            else
            {
                Faces[i].UpdateResolution(R, InSamples, Triangles[i], index);
            }
        }


        index += (R - 1) * (R - 2);

        QPair<int, int> Edges[3];
        Edges[0] = qMakePair(Triangles[i].z(),Triangles[i].y());
        Edges[1] = qMakePair(Triangles[i].y(),Triangles[i].x());
        Edges[2] = qMakePair(Triangles[i].x(),Triangles[i].z());

        for(size_t i = 0; i < 3; i++)
        {
            int edgeindex = index;

            const QPair<int, int> invertedPair = qMakePair(Edges[i].second, Edges[i].first);

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

        const int Res = ssbo->Resolution[i];
        const int nElementInRow = (Res+1);

        //Faces[i].samples[Res][Res] = int(Triangles[i].x());
        //Faces[i].samples[Res][0]   = int(Triangles[i].y());
        //Faces[i].samples[0][0]     = int(Triangles[i].z());

        ssbo->Colors[i][Res * nElementInRow + 0] = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[Res][Res]];
        ssbo->Colors[i][Res]                     = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[Res][0]];
        ssbo->Colors[i][0]                       = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[0][0]];

        int edgeIndexOffset = 0;

        const int ColorsPerEdge = ((Resolutions[i] - 1) * (Resolutions[i] - 2))/2 - 1;

        // Cij => C0k, Ck0, Ck(R-k) => 0 < k < R
        for(int a = 1; a < Res; a++)
        {
            //Faces[i].samples[a][0]   = int(TriangleData.EdgeInfo[0].first + (TriangleData.EdgeInfo[0].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset));
            //Faces[i].samples[Res][a] = int(TriangleData.EdgeInfo[1].first + (TriangleData.EdgeInfo[1].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset));
            //Faces[i].samples[a][a]   = int(TriangleData.EdgeInfo[2].first + (!TriangleData.EdgeInfo[2].second ? ColorsPerEdge - edgeIndexOffset : edgeIndexOffset));

            ssbo->Colors[i][a]                           = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[a][0]];
            ssbo->Colors[i][a * nElementInRow + (Res-a)] = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[Res][a]];
            ssbo->Colors[i][a * nElementInRow]           = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);//InSamples[Faces[i].samples[a][a]];
            edgeIndexOffset++;
        }

        int faceIndexOffset = 0;
        // 0 < a < R, 0 < j < R, a + j != R
        for(int a = 1; a < Res; a++) {
            for(int j = 1; j < Res; j++) {
                if(a + j != Res) {
                    ssbo->Colors[i][a * Res + j] = QVector4D(float(a)/float(Res), float(j)/float(Res), 1.0f, 1.0f);//InSamples[int(TriangleData.FaceIndex + faceIndexOffset)];
                    faceIndexOffset++;
                }
            }
        }

        qDebug() << faceIndexOffset << faceIndexOffset + edgeIndexOffset * 3 + 3;

        faceIndexOffset = 0;
        // 0 < a < R, 0 < j < R, a + j != R
        for(int a = 0; a < Faces[i].samples.size(); a++) {
            for(int j = 0; j < Faces[i].samples[a].size(); j++) {

                ssbo->Colors[i][a * Res + j] =  InSamples[Faces[i].samples[a][j]];
                qDebug() << a << "*" << Res << " + "<< j << " = "<< a * nElementInRow + j;
                faceIndexOffset++;
            }
        }
        qDebug() << faceIndexOffset;
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
