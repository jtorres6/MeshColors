/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "basic_object3d.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

const int MAX_TRIANGLES = 9000;
const int MAX_SAMPLES = 16;

typedef struct ssbo_data
{
    int Resolution[MAX_TRIANGLES];
    QVector4D Colors[MAX_TRIANGLES][MAX_SAMPLES][MAX_SAMPLES];
}ssbo_type;

static int vertexIndex = 0;
typedef struct meshColorsFace
{
    int Resolution;
    QVector<QVector<int>> samples;

    meshColorsFace(const int res)
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

    void UpdateResolution(int newResolution, QVector<QVector4D>& InSamples, const QVector3D& Vertex, const int baseIndex)
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

                //samples[0][0]          = 0;
                //InSamples[samples[0][0]] = QVector4D(1.0f, 0.0f, 1.0f, 1.0f);
                //
                //samples[0][Resolution]          = 1;
                //InSamples[samples[0][Resolution]] = QVector4D(1.0f, 0.0f, 1.0f, 1.0f);
                //
                //samples[Resolution][Resolution] = 2;
                //InSamples[samples[Resolution][Resolution]] = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);

                int edgeIndexOffset = 0;

                // Cij => C0k, Ck0, Ck(R-k) => 0 < k < R
                for(int a = 0; a <= Resolution; a++)
                {
                    samples[a][0]          = Edges[0] + edgeIndexOffset;
                    samples[Resolution][a] = Edges[1] + edgeIndexOffset;
                    samples[a][a]          = Edges[2] + edgeIndexOffset;

                    edgeIndexOffset++;
                }

                int faceIndexOffset = 0;
                // 0 < a < R, 0 < j < R, a + j != R
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

            for (int i = 0; i < samples.size(); ++i) {

                QString row =  "| ";

                for (int j = 0; j < samples[i].size(); j++){
                    bool core = false;
                    for(int a = 0; a < coreSamples.size(); ++a) {
                        if(i == coreSamples[a].first && j == coreSamples[a].second)
                        {
                            //InSamples[samples[coreSamples[i].first][coreSamples[i].second]] = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
                            core = true;
                            break;
                        }
                    }
                }
            }
        }
    }
} meshColorsFace;

struct face_data
{
    int FaceIndex;
    QPair<int, bool> EdgeInfo[3];

    face_data(int InFaceIndex, QPair<int, bool> (InEdgeInfo)[3])
    {
        FaceIndex = InFaceIndex;
        EdgeInfo[0] = InEdgeInfo[0];
        EdgeInfo[1] = InEdgeInfo[1];
        EdgeInfo[2] = InEdgeInfo[2];
    }

    face_data()
    {
        FaceIndex = -1;
        EdgeInfo[0] = QPair<int, bool>(0,false);
        EdgeInfo[1] = QPair<int, bool>(0,false);
        EdgeInfo[2] = QPair<int, bool>(0,false);
    }
};


class _object3D:public _basic_object3D
{
    public:
    QVector<QVector3D> Triangles;
    QVector<QVector3D> VerticesDrawArrays;
    QVector<QVector3D> MeshColorsData;
    QVector<QVector3D> Index;
    QVector<QVector4D> TriangleSelectionColors;
    QVector<QVector4D> VerticesNormals;
    QVector<QVector3D> TrianglesDrawArrays;

    QVector<meshColorsFace> Faces;

    QVector<int> Resolutions;

    _object3D();
    _object3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles);

    void ReadPlyFile(const char *Filename);

    int SsboSize();

    void UpdateMeshColorsArray(QVector<QVector4D>& InSamples);
    void UpdateResolutionsArray(const QVector<int>& InNewResolutions);

    bool TriangleSelectionMode = true;

    QVector<QVector4D> points, selectionPoints;

    QVector<ssbo_data> MeshColorArray;
    QVector<ssbo_data> MeshSelectionArray;
    QVector<QVector4D> ColorArray;

    ssbo_data* ssbo;

    QMatrix4x4 Projection;
    QMatrix4x4 Rotation_x;
    QMatrix4x4 Rotation_y;
    QMatrix4x4 Translation;

};

#endif // OBJECT3D_H
