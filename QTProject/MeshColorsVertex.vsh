#version 420

uniform mat4 matrix;

in vec3 vertex;
in vec4 color;
in vec3 indexes;
in vec4 normals;

out vec4 geometry_Color;
out int VertexIndex;
out vec4 VertexNormal;

void main(void)
{
    geometry_Color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexIndex = int(indexes.x);
    VertexNormal = normals;
}
