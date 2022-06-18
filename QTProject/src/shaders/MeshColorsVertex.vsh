#version 430

uniform mat4 matrix;

in vec3 vertex;
in vec4 color;
in vec3 indexes;
in vec3 normals;

out vec4 geometry_Color;
out vec3 VertexNormal;

void main(void)
{
    geometry_Color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexNormal = normals;
}
