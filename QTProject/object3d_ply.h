#ifndef OBJECT3DPLY_H
#define OBJECT3DPLY_H

#include "object3d.h"
#include "file_ply_stl.h"
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QVector4D>

class object3DPly : public _object3D
{
public:
    object3DPly();
    object3DPly(const char *Filename);

    void Read(const char *Filename);

    vector<GLfloat> triangleVertices;
    vector<QVector4D> colors;
    GLfloat* triangles;
};

#endif // OBJECT3DPLY_H
