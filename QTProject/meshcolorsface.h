#ifndef MESHCOLORSFACE_H
#define MESHCOLORSFACE_H

#include <QVector>
#include <QVector4D>

class MeshColorsFace
{
public:
    MeshColorsFace();
    MeshColorsFace(const int res);
    void UpdateResolution(int newResolution, QVector<QVector4D>& InSamples, const int baseIndex);

    int Resolution;
    QVector<QVector<int>> samples;
};

#endif // MESHCOLORSFACE_H
