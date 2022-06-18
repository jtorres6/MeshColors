#version 430

uniform mat4 matrix;

in vec3 vertex;
in vec3 normals;

out vec3 VertexNormal;

void main(void)
{
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexNormal = normals;
}
