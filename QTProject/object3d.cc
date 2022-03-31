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
#include "meshcolorsface.h"

using namespace _colors_ne;

_object3D::_object3D()
{
}

_object3D::_object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles)
{
    Triangles = InTriangles;
    vertices = InVertices;

    VerticesDrawArrays.resize(Triangles.size()*3);

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
}
