/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "tetrahedron.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_tetrahedron::_tetrahedron(float Size)
{
    Vertices.resize(4);

    Vertices[0]= QVector3D(-Size/2, -Size/2,-Size/2);
    Vertices[1]= QVector3D(0, Size/2, Size/2);
    Vertices[2]= QVector3D(Size/2, Size/2, Size/2);
    Vertices[3]= QVector3D(0, Size/2, 0);

    Triangles.resize(4);

    Triangles[0]= QVector3D(0, 1, 3);
    Triangles[1]= QVector3D(1, 2, 3);
    Triangles[2]= QVector3D(2, 0 ,3);
    Triangles[3]= QVector3D(0, 2, 1);

    VerticesDrawArrays.resize(Triangles.size()*3);

    for (unsigned int i= 0;i<Triangles.size(); i++)
    {
        VerticesDrawArrays[i*3]=Vertices[Triangles[i].x()] ;
        VerticesDrawArrays[i*3+1]=Vertices[Triangles[i].y()] ;
        VerticesDrawArrays[i*3+2]=Vertices[Triangles[i].z()] ;
    }
}
