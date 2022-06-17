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
#include "meshcolorsface.h"

_object3D::_object3D()
{
}

_object3D::_object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles)
    : triangles(InTriangles)
{
    vertices = InVertices;

    verticesDrawArrays.resize(triangles.size() * 3);

    for (int i= 0; i < triangles.length(); i++) {
        verticesDrawArrays[i * 3]   = vertices[triangles[i].z()];
        verticesDrawArrays[i * 3+1] = vertices[triangles[i].y()];
        verticesDrawArrays[i * 3+2] = vertices[triangles[i].x()];

        trianglesDrawArrays.push_back(vertices[triangles[i].z()]);
        trianglesDrawArrays.push_back(vertices[triangles[i].y()]);
        trianglesDrawArrays.push_back(vertices[triangles[i].y()]);
        trianglesDrawArrays.push_back(vertices[triangles[i].x()]);
        trianglesDrawArrays.push_back(vertices[triangles[i].x()]);
        trianglesDrawArrays.push_back(vertices[triangles[i].z()]);

        // Convert "i", the integer mesh ID, into an RGB color
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;

        QVector4D TriangleID = QVector4D(r/255.0f, g/255.0f, b/255.0f, 255.0f);
        TriangleSelectionColors.push_back(TriangleID);
        TriangleSelectionColors.push_back(TriangleID);
        TriangleSelectionColors.push_back(TriangleID);

        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));
        Index.push_back(QVector3D(i, i, i));

        const QVector3D v1 = vertices[int(triangles[i].x())] - vertices[int(triangles[i].y())];
        const QVector3D v2 = vertices[int(triangles[i].y())] - vertices[int(triangles[i].z())];
        const QVector3D& Normal = QVector3D::crossProduct(v1, v2).normalized();

        VerticesNormals.push_back(Normal);
        VerticesNormals.push_back(Normal);
        VerticesNormals.push_back(Normal);
    }
}

