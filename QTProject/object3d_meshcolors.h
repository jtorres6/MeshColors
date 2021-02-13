#ifndef OBJECT3D_MESHCOLORS_H
#define OBJECT3D_MESHCOLORS_H

#include "object3d_ply.h"
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class object3DMeshColors : public object3DPly
{
public:

    vector<QVector3D> ColorData;
    vector<int> resolutions;

    QOpenGLShaderProgram* program;
    object3DMeshColors(const char *Filename);
};

#endif // OBJECT3D_MESHCOLORS_H
