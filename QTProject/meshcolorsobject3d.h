#ifndef MESHCOLORSOBJECT3D_H
#define MESHCOLORSOBJECT3D_H

#include <object3d.h>
#include "meshcolorsface.h"

const int MAX_SAMPLES = 16;

typedef struct ssbo_data
{
    int Resolution[MAX_TRIANGLES];
    QVector4D Colors[MAX_TRIANGLES][MAX_SAMPLES][MAX_SAMPLES];
}ssbo_type;

class MeshColorsObject3D : public _object3D
{
public:
    MeshColorsObject3D();
    MeshColorsObject3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles);

    int GetSsboSize() const;

    void UpdateMeshColorsArray(QVector<QVector4D>& InSamples);
    void UpdateResolutionsArray(const QVector<int>& InNewResolutions);

    ssbo_data* ssbo;
    QVector<QVector4D> Points, SelectionPoints;

private:
    QVector<MeshColorsFace> Faces;

    QVector<ssbo_data> MeshColorArray;
    QVector<ssbo_data> MeshSelectionArray;

    bool TriangleSelectionMode = true;
    QVector<QVector4D> ColorArray;
};

#endif // MESHCOLORSOBJECT3D_H
