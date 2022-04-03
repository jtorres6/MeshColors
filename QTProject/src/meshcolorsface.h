#ifndef MESHCOLORSFACE_H
#define MESHCOLORSFACE_H

#include <QVector>
#include <QVector4D>

class MeshColorsFace
{
public:
    MeshColorsFace();
    MeshColorsFace(const int res);
    MeshColorsFace(const int res, QVector<QVector4D>& colorSamples, const int baseIndex);
    void updateResolution(int res, QVector<QVector4D>& colorSamples);

    int resolution;
    QVector<QVector<int>> samples;

private:
    void initializeSamples();
    void increaseSamples(const int newResolution, QVector<QVector4D>& colorSamples);
    void decreaseSamples(const int newResolution, QVector<QVector4D>& colorSamples);
    void updateSamplesIndexes(QVector<QPair<int, int>>& coreSamples, QVector<int>& coreSampleIndex);

    int faceIndex;
};

#endif // MESHCOLORSFACE_H
