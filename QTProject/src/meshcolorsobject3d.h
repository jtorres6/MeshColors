#ifndef MESHCOLORSOBJECT3D_H
#define MESHCOLORSOBJECT3D_H

#include <src/object3d.h>
#include "meshcolorsface.h"

const int MAX_SAMPLES = 17;
const int MAX_RES = 16;

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
    QVector<MeshColorsFace> Faces;
    QVector<int> Resolutions;

    friend QDataStream &operator<<(QDataStream &ds, const MeshColorsObject3D &inObj);
    friend QDataStream &operator>>(QDataStream &ds, MeshColorsObject3D &inObj);

    void SyncSSBO();

private:

    QVector<ssbo_data> MeshColorArray;

    bool TriangleSelectionMode = true;
    QVector<QVector4D> ColorArray;

    void InitializeSSBO();
    void InitializePointsArrays();

};

#endif // MESHCOLORSOBJECT3D_H
