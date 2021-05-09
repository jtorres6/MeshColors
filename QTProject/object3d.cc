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
    ply.open(Filename);
    vector<float> Coordinates;
    vector<unsigned int> Positions;
    ply.read(Coordinates, Positions);

    for(int i = 0; i <= Coordinates.size()-3; i+=3)
    {
        Vertices.push_back(QVector3D(Coordinates[i],Coordinates[i+1],Coordinates[i+2]));
    }

    int R = 8;
    int index = Vertices.size() - 1;
    for(int i = 0; i <= Positions.size()-3; i+=3)
    {
        Triangles.push_back(QVector3D(Positions[i],Positions[i+1],Positions[i+2]));
        Resolutions.push_back(R);

        // Colors per face:
        int faceindex = index;

        index += ((R - 1) * (R - 2))/2;
        int edge1index = index;
        QPair<int, int> pair = qMakePair(Positions[i],Positions[i+1]);

        if(pair.first > pair.second)
        {
            pair = qMakePair(pair.second, pair.first);
        }
        if(EdgeIndexMap.contains(pair))
        {
            edge1index = *EdgeIndexMap.find(pair);
        }
        else
        {
            EdgeIndexMap.insert(pair, edge1index);
        }
        qDebug() << "Edge: " << edge1index << "      vertices: " <<  pair.first << pair.second;

        index += R;
        int edge2index = index;
        pair =  qMakePair(Positions[i+1],Positions[i+2]);

        if(pair.first > pair.second)
        {
            pair = qMakePair(pair.second, pair.first);
        }

        if(EdgeIndexMap.contains(pair))
        {
            edge2index = *EdgeIndexMap.find(pair);
        }
        else
        {
            EdgeIndexMap.insert(pair, edge2index);
        }
        qDebug() << "Edge: " << edge2index  << "      vertices: " <<  pair.first << pair.second;

        pair =  qMakePair(Positions[i+2],Positions[i]);
        if(pair.first > pair.second)
        {
            pair = qMakePair(pair.second, pair.first);
        }

        // Colors per edge (R-1)+1:
        index += R;
        int edge3index = index;
        if(EdgeIndexMap.contains(pair))
        {
            edge3index = *EdgeIndexMap.find(pair);
        }
        else
        {
            EdgeIndexMap.insert(pair, edge3index);
        }
        qDebug() << "Edge: " << edge3index  << "      vertices: " <<  pair.first << pair.second;
        index += R;

        PerFaceData.push_back(QVector4D(faceindex,edge2index,edge3index,edge1index));
        PerFaceData.push_back(QVector4D(faceindex,edge2index,edge3index,edge1index));
        PerFaceData.push_back(QVector4D(faceindex,edge2index,edge3index,edge1index));
        qDebug() << index << faceindex << edge2index << edge3index << edge1index;
    }

    //qDebug() << Positions.size()/3;
}

_object3D::_object3D()
{
}

_object3D::_object3D(const char *Filename)
{
    ReadPlyFile(Filename);

    VerticesDrawArrays.resize(Triangles.size()*3);

    QVector<QVector4D> ColorPerVertex;
    for (unsigned int i= 0; i < Triangles.size(); i++)
    {
        ColorPerVertex.push_back(QVector4D((float)i/(float)Triangles.size(), 0.0f, 0.0f, 1.0f));
    }

    int EdgeIndex = Vertices.size();
    for (unsigned int i= 0; i < Triangles.size(); i++)
    {
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

       //Colors.push_back(ColorPerVertex[Triangles[i].x()]);
       //Colors.push_back(ColorPerVertex[Triangles[i].y()]);
       //Colors.push_back(ColorPerVertex[Triangles[i].z()]);

       // Vertex 1
       //Index.push_back(QVector3D(Triangles[i].x(),Triangles[i].y(),Triangles[i].z()));
       //PerFaceData.push_back(QVector4D(3,4,5,6));
       //// Vertex 1
       //Index.push_back(QVector3D(Triangles[i].x(),Triangles[i].y(),Triangles[i].z()));
       //PerFaceData.push_back(QVector4D(3,4,5,6));
       //// Vertex 1
       //Index.push_back(QVector3D(Triangles[i].x(),Triangles[i].y(),Triangles[i].z()));
       //PerFaceData.push_back(QVector4D(3,4,5,6));
       // Vertex 1
       Index.push_back(QVector3D(Triangles[i].x(),Triangles[i].x(),Triangles[i].x()));
       //PerFaceData.push_back(QVector4D(Vertices.size()+i,15,15,15));
       // Vertex 1
       Index.push_back(QVector3D(Triangles[i].y(),Triangles[i].y(),Triangles[i].y()));
       //PerFaceData.push_back(QVector4D(15,15,15,15));
       // Vertex 1
       Index.push_back(QVector3D(Triangles[i].z(),Triangles[i].z(),Triangles[i].z()));
       //PerFaceData.push_back(QVector4D(15,15,15,15));
    }
}

