#version 450 core

uniform mat4 matrix;

in vec3 vertex;
in vec4 color;
in vec3 indexes;

out vec4 out_color;
out int VertexIndex;
void main(void)
{
    out_color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexIndex = int(indexes.x);
}
