#version 450 core
layout (location = 3) uniform highp vec3 points[255];

const int R = 4;

in vec3 fragIndexes;
in vec4 fragFaceIndexes;
in vec4 out_color;
out vec4 frag_color;

void main(void)
{
    const int IndexSize = int(ceil(3 + 3*(R-1) + (R-1)*(R-2)*0.5));

    vec4 color = out_color;
    int PointsIndex[R+1][R+1];
    vec4 B =  floor(R*color);
    vec4 w =(R*color)-B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    PointsIndex[0][0] = int(fragIndexes.x);
    PointsIndex[0][R] = int(fragIndexes.y);
    PointsIndex[R][0] = int(fragIndexes.z);

    for(int i = 1; i < R-1; i++)
    {
        PointsIndex[0][i] = int(fragFaceIndexes.r);
        PointsIndex[i][0] = int(fragFaceIndexes.g);
        PointsIndex[i][R-i] = int(fragFaceIndexes.b);
    }

    PointsIndex[1][1] = int(fragFaceIndexes.a);
    PointsIndex[3][2] = int(fragFaceIndexes.a);
    PointsIndex[2][3] = int(fragFaceIndexes.a);

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


    gl_FragColor = c;
}
