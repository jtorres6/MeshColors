#version 450 core

layout (location = 0) uniform mat4 matrix;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 color;

out vec4 next_color;
void main(void)
{
    next_color = color;
    gl_Position = matrix*vec4(vertex, 1.0f);
}
