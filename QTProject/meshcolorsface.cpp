#include "meshcolorsface.h"
#include <QVector4D>

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
        QVector<QPair<int, int>> coreSamples;
        QVector<int> coreSampleIndex;

        if(newResolution != resolution) {
            if (newResolution > resolution) {
                increaseSamples(newResolution, coreSamples, coreSampleIndex);
            }
            else {
                decreaseSamples(newResolution, coreSamples, coreSampleIndex);
            }

            ////////////////////////////////////////////////////////////////////////////

            resolution = newResolution;

            updateSamplesIndexes(coreSamples, coreSampleIndex);

            for(int a = 0; a < samples.size(); a++) {
                for(int j = 0; j < samples[a].size(); j++) {
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

                            if (a+1 <= samples.size()) {
                                newColor += colorSamples[samples[a+1][j]];
                                ++colorCount;
                            }
                        }

                        if (j%2 != 0)
                        {
                            if (j-1 >= 0) {
                                newColor += colorSamples[samples[a][j-1]];
                                ++colorCount;
                            }

                            if (j+1 <= samples.size()) {
                                newColor += colorSamples[samples[a][j+1]];
                                ++colorCount;
                            }
                        }

                        colorSamples[samples[a][j]] = newColor/colorCount;
                    }
                }
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

void MeshColorsFace::increaseSamples(const int newResolution,  QVector<QPair<int, int>>& coreSamples, QVector<int>& coreSampleIndex)
{
    QVector<QVector<int>> oldSamples = samples;
    const int step = newResolution/resolution;

    for (int i = 0; i < samples.size(); ++i) {
        for (int j = 0; j < samples[i].size(); j++){
            coreSamples.push_back({i * step, j * step});
            coreSampleIndex.push_back(samples[i][j]);
        }
    }

    for (int i = 0; i <= newResolution; ++i) {

        if(i > resolution)
        {
            QVector<int> indexVector;
            for (int j = 0; j <= newResolution; j++){
                indexVector.push_back(faceIndex);
                ++faceIndex;
            }

            samples.push_back(indexVector);
        }
        else
        {
            for (int j = resolution + 1; j <= newResolution; j++){
                samples[i].push_back(faceIndex);
                ++faceIndex;
            }
        }
    }
}

void MeshColorsFace::decreaseSamples(const int newResolution, QVector<QPair<int, int>>& coreSamples, QVector<int>& coreSampleIndex)
{
    QVector<QVector<int>> newSamples;
    const int step = newResolution/resolution;

    for (int i = 0; i < samples.size(); ++i) {
        if (i%2 == 0.0f) {

            QVector<int> row;

            for (int j = 0; j < samples[i].size(); j++){
                if (j%2 == 0.0f) {
                    row.push_back(samples[i][j]);
                    coreSamples.push_back({i * step, j * step});
                    coreSampleIndex.push_back(samples[i][j]);
                }
            }

            newSamples.push_back(row);
        }
    }

    samples = newSamples;
}

void MeshColorsFace::updateSamplesIndexes(QVector<QPair<int, int>>& coreSamples, QVector<int>& coreSampleIndex)
{
    int index = (faceIndex * 32 * 32)/2 + (resolution - 1) * (resolution - 2);

    QVector<int> Edges;
    Edges.push_back(index);
    index += resolution-1;
    Edges.push_back(index);
    index += resolution-1;
    Edges.push_back(index);
    index += resolution-1;

    int edgeIndexOffset = 0;
    for(int a = 0; a <= resolution; a++)
    {
        samples[a][0]          = Edges[0] + edgeIndexOffset;
        samples[resolution][a] = Edges[1] + edgeIndexOffset;
        samples[a][a]          = Edges[2] + edgeIndexOffset;

        edgeIndexOffset++;
    }

    int faceIndexOffset = 0;
    for(int a = 0; a < samples.size(); a++) {
        for(int j = 0; j < samples[a].size(); j++) {
            samples[a][j]   = index + faceIndexOffset;
            faceIndexOffset++;
        }
    }

    for(int i = 0; i < coreSamples.size(); ++i) {
        samples[coreSamples[i].first][coreSamples[i].second] = coreSampleIndex[i];
    }
}
