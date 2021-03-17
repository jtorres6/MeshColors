#version 450 core

layout (location = 0) uniform mat4 matrix;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 color;
layout (location = 3) uniform highp vec3 points[255];
layout (location = 4) in vec3 indexes;
layout (location = 5) in vec4 faceIndexes;
out vec3 fragIndexes;
out vec4 fragFaceIndexes;

out vec4 out_color;
void main(void)
{
    fragIndexes = indexes;
    fragFaceIndexes = faceIndexes;
    out_color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
}
