#version 460

uniform mat4 matrix;

in vec3 vertex;
in vec4 color;
in vec3 indexes;

out vec4 geometry_Color;
out int VertexIndex;
void main(void)
{
    geometry_Color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
    VertexIndex = int(indexes.x);
}
