#include "object3d_meshcolors.h"

float MAX_DATA = 10000;
/*****************************************************************************//**
*
*
*
*****************************************************************************/


object3DMeshColors::object3DMeshColors(const char *Filename)
    : object3DPly(Filename)
{
    ColorData = vector<QVector3D>();

    ColorData.resize(MAX_DATA);

}
