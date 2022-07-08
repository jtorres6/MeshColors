#include "meshcolorsface.h"
#include <QVector4D>
#include <QDebug>
#include <QDataStream>

static int vertexIndex = 0;

MeshColorsFace::MeshColorsFace()
    : resolution(0)
    , faceIndex(0)
{
}

MeshColorsFace::MeshColorsFace(const int res)
    : resolution(res)
    , faceIndex(0)
{
    initializeSamples();
}

MeshColorsFace::MeshColorsFace(const int res, QVector<QVector4D>& colorSamples, const int baseindex)
    : resolution(res)
    , faceIndex(baseindex)
{
    initializeSamples();
    updateResolution(res, colorSamples);
}

void MeshColorsFace::updateResolution(const int newResolution, QVector<QVector4D>& colorSamples)
{
    if(newResolution != resolution && resolution != 0 && newResolution != 0)
    {
        if(newResolution != resolution) {
            if (newResolution > resolution) {
                increaseSamples(newResolution, colorSamples);
            }
            else {
                decreaseSamples(newResolution, colorSamples);
            }
        }
    }
}

void MeshColorsFace::initializeSamples()
{
    for (int i = 0; i <= resolution; ++i) {
        QVector<int> indexVector;

        for (int j = 0; j <= resolution; ++j){
            indexVector.push_back(vertexIndex);
            ++vertexIndex;
        }

        samples.push_back(indexVector);
    }
}

void MeshColorsFace::increaseSamples(const int newResolution, QVector<QVector4D>& colorSamples)
{
    QVector<QPair<int, int>> coreSamples;
    QVector<int> coreSampleIndex;

    const int step = newResolution/resolution;

    for (int i = 0; i < samples.size(); ++i) {
        for (int j = 0; j < samples[i].size(); j++){
            coreSamples.push_back({i * step, j * step});
            coreSampleIndex.push_back(samples[i][j]);
        }
    }

    int auxIndex = faceIndex;
    for (int i = 0; i <= newResolution; ++i) {

        if(i > resolution)
        {
            QVector<int> indexVector;
            for (int j = 0; j <= newResolution; j++){
                indexVector.push_back(auxIndex);
                ++auxIndex;
            }

            samples.push_back(indexVector);
        }
        else
        {
            for (int j = resolution + 1; j <= newResolution; j++){
                samples[i].push_back(auxIndex);
                ++auxIndex;
            }
        }
    }

    resolution = newResolution;

    updateSamplesIndexes(coreSamples, coreSampleIndex);

    for(int a = 0; a < samples.size(); a++) {
        for(int j = 0; j < samples[a].size() - a; j++) {
            if (!coreSamples.contains({a, j}))
            {
                QVector4D newColor = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);

                int colorCount = 0;

                if (a%2 != 0)
                {
                    if (a-1 >= 0) {
                        newColor += colorSamples[samples[a-1][j]];
                        ++colorCount;
                    }

                    if (a+1 < samples.size() && j < samples[a+1].size() - a+1) {
                        if(coreSamples.contains({a+1,j}))
                        {
                            newColor += colorSamples[samples[a+1][j]];
                            ++colorCount;
                        }
                    }
                }

                if (j%2 != 0)
                {
                    if (j-1 >= 0) {
                        newColor += colorSamples[samples[a][j-1]];
                        ++colorCount;
                    }

                    if (j+1 < samples[a].size() - a) {
                        if(coreSamples.contains({a, j+1}))
                        {
                            newColor += colorSamples[samples[a][j+1]];
                            ++colorCount;
                        }
                    }
                }

                if(colorCount > 0) {
                    newColor = newColor/colorCount;
                    newColor[3] = 1.0f;
                    int x = samples[a][j];
                    colorSamples[x] = newColor;
                }
            }
        }
    }
}

void MeshColorsFace::decreaseSamples(const int newResolution, QVector<QVector4D>& colorSamples)
{
    QVector<QPair<int, int>> coreSamples;
    QVector<int> coreSampleIndex;

    QVector<QVector<int>> newSamples;
    const float step = float(newResolution)/float(resolution);

    for (int i = 0; i < samples.size(); ++i) {
        if (i%2 == 0.0f) {

            QVector<int> row;

            for (int j = 0; j < samples[i].size(); j++){
                if (j%2 == 0.0f) {
                    row.push_back(samples[i][j]);
                    coreSamples.push_back({int(i * step), int(j * step)});
                    coreSampleIndex.push_back(samples[i][j]);

                    QVector4D newColor = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);

                    int count = 0;

                    for(int k = -1; k <= 1; ++k) {
                        for(int a = -1; a <= 1; ++a) {
                            if(i+k < samples.size() && i+k >= 0) {
                                const int size = samples[i+k].size();
                                if (j+a < size - (i+k) && j+a >= 0) {
                                    newColor += colorSamples[samples[i+k][a+j]];
                                    ++count;
                                }
                            }
                        }
                    }
                    if(count > 0) {
                        colorSamples[samples[i][j]] = newColor/count;
                    }
                }
            }

            newSamples.push_back(row);
        }
    }

    samples = newSamples;

    resolution = newResolution;

    updateSamplesIndexes(coreSamples, coreSampleIndex);
}

void MeshColorsFace::updateSamplesIndexes(QVector<QPair<int, int>>& coreSamples, QVector<int>& coreSampleIndex)
{
    int index = (faceIndex * 32 * 32)/2 + (resolution - 1) * (resolution - 2);

    int faceIndexOffset = 0;
    for(int a = 0; a < samples.size(); ++a) {
        for(int j = 0; j < samples[a].size(); ++j) {
            samples[a][j] = index + faceIndexOffset;
            faceIndexOffset++;
        }
    }

    for(int i = 0; i < coreSamples.size(); ++i) {
        samples[coreSamples[i].first][coreSamples[i].second] = coreSampleIndex[i];
    }
}

QDataStream &operator<<(QDataStream &ds, const MeshColorsFace &inObj)
{
    ds << inObj.resolution;
    ds << inObj.samples;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, MeshColorsFace &inObj)
{
    ds >> inObj.resolution;
    ds >> inObj.samples;

    return ds;
}

