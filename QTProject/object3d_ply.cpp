#include "object3d_ply.h"

void object3DPly::Read(const char *Filename)
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

   for(int i = 0; i <= Positions.size()-3; i+=3)
   {
        Triangles.push_back(QVector3D(Positions[i],Positions[i+1],Positions[i+2]));

   }

   VerticesDrawArrays.resize(Triangles.size()*3);

   for (unsigned int i= 0;i<Triangles.size(); i++)
   {
       VerticesDrawArrays[i*3]=Vertices[Triangles[i].x()] ;
       VerticesDrawArrays[i*3+1]=Vertices[Triangles[i].y()] ;
       VerticesDrawArrays[i*3+2]=Vertices[Triangles[i].z()] ;

       colors.push_back(QVector4D(1.0, 0.0, 0.0, 0.0));
       colors.push_back(QVector4D(0.0, 1.0, 0.0, 0.0));
       colors.push_back(QVector4D(0.0, 0.0, 1.0, 0.0));
   }
}

object3DPly::object3DPly()
{
}


object3DPly::object3DPly(const char *Filename)
{
    Read(Filename);
}
