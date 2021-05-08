#version 450 core
layout (location = 3) uniform highp vec3 points[255];

const int R = 4;

flat in int Index[3];
in vec4 out_fragFaceIndexes;
in vec4 next_color;
out vec4 frag_color;

uniform bool ColorLerpEnabled;

void main(void)
{
    // BEGIN_TODO: Move this code to the geometr shader
    int PointsIndex[R+1][R+1];
    PointsIndex[R][0] = Index[0];
    PointsIndex[0][R] = Index[1];
    PointsIndex[0][0] = Index[2];
    int faceIndexOffset = 0;
    int edgeIndexOffset = 0;
    for(int i = 1; i < R; i++)
    {
        PointsIndex[0][i] = int(out_fragFaceIndexes.g + i);
        PointsIndex[i][0] = int(out_fragFaceIndexes.b + i);
        for(int j = 1; j < R; j++)
        {
            if(i + j == R)
            {
                PointsIndex[i][j] = int(out_fragFaceIndexes.a + edgeIndexOffset);
                edgeIndexOffset += 1;
            }
            else
            {
                PointsIndex[i][j] = int(out_fragFaceIndexes.r + faceIndexOffset);
                faceIndexOffset += 1;
            }
        }
    }
    // END_TODO

    vec4 color = next_color;
    vec4 B =  floor(R * color);
    vec4 w = (R * color) - B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    if(ColorLerpEnabled)
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = vec4(points[PointsIndex[int(B[0])][int(B[1])]], 1.0f);
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            vec3 c1 = w.r*points[PointsIndex[int(B.r+1)][int(B.g)]];
            vec3 c2 = w.g*points[PointsIndex[int(B.r)][int(B.g+1)]];
            vec3 c3 = w.b*points[PointsIndex[int(B.r)][int(B.g)]];
            c = vec4(c1+c2+c3, 1.0f);
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            vec3 c1 = (1-w.r)*points[PointsIndex[int(B.r)][int(B.g+1)]];
            vec3 c2 = (1-w.g)*points[PointsIndex[int(B.r+1)][int(B.g)]];
            vec3 c3 = (1-w.b)*points[PointsIndex[int(B.r+1)][int(B.g+1)]];
            c =  vec4(c1+c2+c3, 1.0f);
        }
    }
    else
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = vec4(points[PointsIndex[int(B[0])][int(B[1])]], 1.0f);
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            float maxc = max(max(w.r, w.g), w.b);

            if(maxc == w.r)
            {
                c = vec4(points[PointsIndex[int(B.r+1)][int(B.g)]], 1.0f);
            }
            else if(maxc == w.g)
            {
                c = vec4(points[PointsIndex[int(B.r)][int(B.g+1)]], 1.0f);
            }
            else if(maxc == w.b)
            {
                c = vec4(points[PointsIndex[int(B.r)][int(B.g)]], 1.0f);
            }
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            float maxc = max(max((1-w.r), (1-w.g)), (1-w.b));

            if(maxc == (1-w.r))
            {
                c = vec4(points[PointsIndex[int(B.r)][int(B.g+1)]], 1.0f);
            }
            else if(maxc == (1-w.g))
            {
                c = vec4(points[PointsIndex[int(B.r+1)][int(B.g)]], 1.0f);
            }
            else if(maxc == (1-w.b))
            {
                c = vec4(points[PointsIndex[int(B.r+1)][int(B.g+1)]], 1.0f);
            }
        }
    }


    gl_FragColor = c;//vec4(color.r*points[Index[0]] + color.g*points[Index[1]] + color.b*points[Index[2]], 1.0f);
}
