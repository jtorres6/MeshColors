#version 450 core

uniform mat4 matrix;
in vec3 vertex;
in vec4 color;
in vec3 indexes;
in vec4 faceIndexes;
in int resolution;
//flat out vec3 fragIndexes;
flat out vec4 fragFaceIndexes;
out vec3 vIndexes;

out vec4 out_color;
flat out int res;
out int VertexIndex;
void main(void)
{
    res = resolution;
    vIndexes = indexes;
    fragFaceIndexes = faceIndexes;
    out_color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexIndex = int(indexes.x);
}
