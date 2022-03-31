/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "object3d.h"
#include <QDebug>
#include <map>
#include <QDebug>

using namespace _colors_ne;

_object3D::_object3D()
{
}

_object3D::_object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles)
{
    Triangles = InTriangles;
    vertices = InVertices;

    ssbo = (ssbo_data*)malloc(sizeof(ssbo_data));

    VerticesDrawArrays.resize(Triangles.size()*3);

    for(size_t i = 0; i < MAX_TRIANGLES*MAX_SAMPLES*MAX_SAMPLES; i++)
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

float RandomFloat(const float a, const float b) {
    const float random = ((float) rand()) / (float) RAND_MAX;
    const float diff = b - a;
    const float r = random * diff;
    return a + r;
}

void _object3D::UpdateMeshColorsArray(QVector<QVector4D>& InSamples)
{
    if(ssbo == nullptr)
        return;

    QMap<QPair<int,int>, int> EdgeIndexMap;
    face_data TriangleData;

    for(int i = 0; i < Triangles.size(); i++)
    {
        // Colors per face:
        int R = 4;

        if(ssbo->Resolution[i] <= MAX_SAMPLES)
        {
            R = ssbo->Resolution[i];

            if(i >= Faces.size())
            {
                meshColorsFace m = meshColorsFace(R);
                m.UpdateResolution(R, InSamples, Triangles[i], i);
                Faces.push_back(m);
            }
            else
            {
                Faces[i].UpdateResolution(R, InSamples, Triangles[i], i);
            }
        }

        for(int a = 0; a < Faces[i].samples.size(); a++) {
            for(int j = 0; j < Faces[i].samples[a].size(); j++) {
                ssbo->Colors[i][a][j] =  InSamples[Faces[i].samples[a][j]];
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
