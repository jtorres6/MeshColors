#version 450 core

layout (location = 0) uniform mat4 matrix;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 color;
layout (location = 4) in vec3 indexes;
layout (location = 5) in vec4 faceIndexes;
//flat out vec3 fragIndexes;
flat out vec4 fragFaceIndexes;
out vec3 vIndexes;

out vec4 out_color;
out int VertexIndex;
void main(void)
{
    vIndexes = indexes;
    fragFaceIndexes = faceIndexes;
    out_color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexIndex = int(indexes.x);
}
