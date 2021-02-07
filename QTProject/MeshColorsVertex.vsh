#version 450 core

layout (location = 0) uniform mat4 matrix;
layout (location = 1) uniform vec3 vertex;
layout (location = 2) uniform vec4 color;

out vec4 out_color;
void main(void)
{
    out_color = color;
    gl_Position = matrix*vec4(vertex,1.0f);
}
