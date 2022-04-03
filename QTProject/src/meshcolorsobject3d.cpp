#include "meshcolorsobject3d.h"

MeshColorsObject3D::MeshColorsObject3D()
    : _object3D()
{
    InitializeSSBO();
}

MeshColorsObject3D::MeshColorsObject3D(QVector<QVector3D>& InVertices, QVector<QVector3D>& InTriangles)
    : _object3D(InVertices, InTriangles)
{
    InitializeSSBO();
    InitializePointsArrays();

    for(int i = 0; i < Triangles.size(); i++)
    {
        Resolutions.push_back(2);
    }

    UpdateResolutionsArray(Resolutions);
    UpdateMeshColorsArray(Points);
}

int MeshColorsObject3D::GetSsboSize() const
{
    return sizeof(ssbo_data);
}

void MeshColorsObject3D::UpdateMeshColorsArray(QVector<QVector4D>& InSamples)
{
    if(ssbo != nullptr){
        for(int i = 0; i < Triangles.size(); i++)
        {
            const int R = ssbo->Resolution[i];

            if(R <= MAX_SAMPLES)
            {
                if(i >= Faces.size())
                {
                    Faces.push_back(MeshColorsFace(R, InSamples, i));
                }
                else
                {
                    Faces[i].updateResolution(R, InSamples);
                }
            }

            for(int a = 0; a < Faces[i].samples.size(); a++) {
                for(int j = 0; j < Faces[i].samples[a].size(); j++) {
                    ssbo->Colors[i][a][j] = InSamples[Faces[i].samples[a][j]];
                }
            }
        }
    }
}

void MeshColorsObject3D::UpdateResolutionsArray(const QVector<int>& InNewResolutions)
{
    if(ssbo != nullptr)
    {
        for(int i = 0; i < Triangles.size(); i++)
        {
            if(i >= InNewResolutions.size()) return;

            ssbo->Resolution[i] = InNewResolutions[i];
        }
    }
}

void MeshColorsObject3D::InitializeSSBO()
{
    ssbo = (ssbo_data*)malloc(sizeof(ssbo_data));
}

void MeshColorsObject3D::InitializePointsArrays()
{
    const size_t SamplesArraySize = MAX_TRIANGLES * 32 * 32;
    for(size_t i = 0; i < SamplesArraySize; i++)
    {
        if(i%2 == 0)
        {
            Points.push_back(QVector4D(0.2f, 0.2f, 0.2f, 1.0f));
        }
        else
        {
            Points.push_back(QVector4D(0.4f, 0.4f, 0.4f, 1.0f));
        }

        // Convert "i", the integer mesh ID, into an RGB color
        const int r = (i & 0x000000FF) >>  0;
        const int g = (i & 0x0000FF00) >>  8;
        const int b = (i & 0x00FF0000) >> 16;

        SelectionPoints.push_back(QVector4D(r/255.0f, g/255.0f, b/255.0f, i/float(SamplesArraySize)));
    }
}

