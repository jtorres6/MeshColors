#include "meshcolorsface.h"
#include <QVector4D>

static int vertexIndex = 0;

MeshColorsFace::MeshColorsFace()
    : Resolution(0)
{
}

MeshColorsFace::MeshColorsFace(const int res)
    : Resolution(res)
{
    for (int i = 0; i <= Resolution; ++i) {

        QVector<int> indexVector;

        for (int j = 0; j <= Resolution; ++j){
            indexVector.push_back(vertexIndex);
            ++vertexIndex;
        }

        samples.push_back(indexVector);
    }
}

void MeshColorsFace::UpdateResolution(int newResolution, QVector<QVector4D>& InSamples, const int baseIndex)
{
    int faceIndex = baseIndex;
    if(newResolution != Resolution && Resolution != 0 && newResolution != 0)
    {
        const float step = float(newResolution)/float(Resolution);
        QVector<QPair<int, int>> coreSamples;
        QVector<int> coreSampleIndex;

        if(step != 0) {
            if (step > 1) {
                QVector<QVector<int>> oldSamples = samples;

                for (int i = 0; i < samples.size(); ++i) {
                    for (int j = 0; j < samples[i].size(); j++){
                        coreSamples.push_back({i * step, j * step});
                        coreSampleIndex.push_back(samples[i][j]);
                    }
                }

                for (int i = 0; i <= newResolution; ++i) {

                    if(i > Resolution)
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
                        for (int j = Resolution + 1; j <= newResolution; j++){
                            samples[i].push_back(faceIndex);
                            ++faceIndex;
                        }
                    }
                }


            }
            else if (step < 1) {
                QVector<QVector<int>> newSamples;

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

            ////////////////////////////////////////////////////////////////////////////

            Resolution = newResolution;
            int index = (baseIndex * 32 * 32)/2 + (Resolution - 1) * (Resolution - 2);

            QVector<int> Edges;
            Edges.push_back(index);
            index += Resolution-1;
            Edges.push_back(index);
            index += Resolution-1;
            Edges.push_back(index);
            index += Resolution-1;

            int edgeIndexOffset = 0;
            for(int a = 0; a <= Resolution; a++)
            {
                samples[a][0]          = Edges[0] + edgeIndexOffset;
                samples[Resolution][a] = Edges[1] + edgeIndexOffset;
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

            for(int a = 0; a < samples.size(); a++) {
                for(int j = 0; j < samples[a].size(); j++) {
                    if (!coreSamples.contains({a, j}))
                    {
                        QVector4D newColor = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);

                        int colorCount = 0;

                        if (a%2 != 0)
                        {
                            if (a-1 >= 0) {
                                newColor += InSamples[samples[a-1][j]];
                                ++colorCount;
                            }

                            if (a+1 <= samples.size()) {
                                newColor += InSamples[samples[a+1][j]];
                                ++colorCount;
                            }
                        }

                        if (j%2 != 0)
                        {
                            if (j-1 >= 0) {
                                newColor += InSamples[samples[a][j-1]];
                                ++colorCount;
                            }

                            if (j+1 <= samples.size()) {
                                newColor += InSamples[samples[a][j+1]];
                                ++colorCount;
                            }
                        }

                        InSamples[samples[a][j]] = newColor/colorCount;
                    }
                }
            }
        }
    }
}


typedef struct meshColorsFace
{

} meshColorsFace;
