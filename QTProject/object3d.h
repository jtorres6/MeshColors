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
const int MAX_SAMPLES = 32;

typedef struct ssbo_data
{
    int Resolution[MAX_TRIANGLES];
    QVector4D Colors[MAX_TRIANGLES][MAX_SAMPLES][MAX_SAMPLES];
}ssbo_type;

typedef struct meshColorsFace
{
    int Resolution;
    QVector<QVector<int>> samples;

    meshColorsFace(const int res)
        : Resolution(res)
    {
        for (int i = 0; i <= Resolution; ++i) {

            QVector<int> indexVector;

            for (int j = 0; j <= Resolution; j++){
                indexVector.push_back(i);
            }

            samples.push_back(indexVector);
        }


        qDebug() << Resolution;
        for (int i = 0; i < samples.size(); ++i) {

            QString row =  "| ";
            for (int j = 0; j <samples[i].size(); j++){
                row += QString::number(i) + ", " + QString::number(j) + " | ";
            }

            qDebug()<< row;
        }
    }

    void UpdateResolution(int newResolution, QVector<QVector4D>& InSamples, const QVector3D& Vertex, const int baseIndex)
    {
        static int faceIndex;
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
                            coreSamples.push_back({i, j});
                            coreSampleIndex.push_back(samples[i][j]);
                        }
                    }

                    for (int i = 0; i <= newResolution; ++i) {

                        if( i > Resolution)
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
                            for (int j = 0; j <= newResolution; j++){
                                samples[i].push_back(faceIndex);
                                ++faceIndex;
                            }
                        }
                    }

                    for (int i = 0; i < samples.size(); ++i) {
                        for (int j = 0; j < samples[i].size(); j++){

                            QVector4D newColor = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
                            int numColorReferences = 0;
                            float distance = MAX_FLOAT_VALUE;

                            for(QPair<int, int> sample : coreSamples) {
                                if(std::abs(i - sample.first * step) + std::abs(j - sample.second * step) <= distance) {
                                    newColor = InSamples[oldSamples[sample.first][sample.second]];
                                    distance = std::abs(i - sample.first * step) + std::abs(j - sample.second * step);

                                    ++numColorReferences;
                                }

                                if(std::abs(i - sample.first * step) == 0 && std::abs(j - sample.second * step) == 0)
                                {
                                    numColorReferences = 0;
                                    //InSamples[samples[i][j]] = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
                                    break;
                                }
                            }

                            if(numColorReferences > 0) {
                                //InSamples[samples[i][j]] = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
                            }
                            else
                            {
                                //InSamples[samples[i][j]] = QVector4D(0.0f, 1.0f, 0.0f, 1.0f);
                            }
                        }
                    }
                }
                else if (step < 1) {
                    QVector<QPair<int, int>> coreSamples;
                    QVector<QVector<int>> newSamples;

                    for (int i = 0; i < samples.size(); ++i) {
                        if (i%2 == 0.0f) {

                            QVector<int> row;

                            for (int j = 0; j < samples[i].size(); j++){
                                if (j%2 == 0.0f) {
                                    row.push_back(samples[i][j]);
                                }
                            }

                            newSamples.push_back(row);
                        }
                    }

                    samples = newSamples;
                }

                ////////////////////////////////////////////////////////////////////////////

                Resolution = newResolution;
                int index = baseIndex + (Resolution - 1) * (Resolution - 2);

                QVector<int> Edges;
                Edges.push_back(index);
                index += Resolution-1;
                Edges.push_back(index);
                index += Resolution-1;
                Edges.push_back(index);
                index += Resolution-1;

                samples[Resolution][Resolution] = int(Vertex.x());
                //InSamples[Vertex.x()] = QVector4D(float(Resolution)/float(Resolution), float(Resolution)/float(Resolution), 0.0f, 1.0f);

                samples[Resolution][0]          = int(Vertex.y());
                //InSamples[Vertex.y()] = QVector4D(float(Resolution)/float(Resolution), 0, 0.0f, 1.0f);

                samples[0][0]                   = int(Vertex.z());
                //InSamples[Vertex.z()] = QVector4D(0, 0, 0.0f, 1.0f);

                int edgeIndexOffset = 0;

                // Cij => C0k, Ck0, Ck(R-k) => 0 < k < R
                for(int a = 0; a <= Resolution; a++)
                {
                    samples[a][0]          = Edges[0] + edgeIndexOffset;
                    //InSamples[Edges[0] + edgeIndexOffset] = QVector4D(float(a)/float(Resolution), 0.0f, 0.0f, 1.0f);

                    samples[Resolution][a] = Edges[1] + edgeIndexOffset;
                    //InSamples[Edges[1] + edgeIndexOffset] = QVector4D(float(Resolution)/float(Resolution), float(a)/Resolution, 0.0f, 1.0f);

                    samples[a][a]          = Edges[2] + edgeIndexOffset;
                    //InSamples[Edges[2] + edgeIndexOffset] = QVector4D(float(a)/float(Resolution), float(a)/float(Resolution), 0.0f, 1.0f);

                    edgeIndexOffset++;
                }

                int faceIndexOffset = 0;
                // 0 < a < R, 0 < j < R, a + j != R
                for(int a = 0; a <= Resolution; a++) {
                    for(int j = 0; j < samples[a].size(); j++) {
                        samples[a][j]   = index + faceIndexOffset;
                        //InSamples[index + faceIndexOffset] = QVector4D(float(a)/float(Resolution), float(j)/float(Resolution), 0.0f, 1.0f);
                        faceIndexOffset++;
                    }
                }

                for(int i = 0; i < coreSamples.size(); ++i) {
                    //samples[coreSamples[i].first* step][coreSamples[i].second * step] = coreSampleIndex[i];
                }
        }

        qDebug() << "\n\n" << Resolution << "\n";
        for (int i = 0; i < samples.size(); ++i) {

            QString row =  "| ";
            for (int j = 0; j < samples[i].size(); j++){
                row += QString::number(i) + ", " + QString::number(j) + " | ";
            }

            qDebug()<< row << "\n";
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
    _object3D(const char *Filename);

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
