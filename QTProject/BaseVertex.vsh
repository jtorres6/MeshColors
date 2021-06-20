#version 450 core

layout (location = 0) uniform mat4 matrix;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 color;
layout (location = 3) uniform vec4 LineColor;
layout (location = 4) uniform bool LineMode;

out vec4 next_color;
void main(void)
{
    if(LineMode)
    {
        next_color = LineColor;
    }
    else
    {
        next_color = color;
    }

    gl_Position = matrix*vec4(vertex, 1.0f);
}
