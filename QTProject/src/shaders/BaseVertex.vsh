#version 430

layout (location = 0) uniform mat4 matrix;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 color;
layout (location = 3) uniform vec4 lineColor;
layout (location = 4) uniform bool singleLineColor;

out vec4 next_color;

void main(void)
{
    if(singleLineColor)
    {
        next_color = lineColor;
    }
    else
    {
        next_color = color;
    }

    gl_Position = matrix*vec4(vertex, 1.0f);
}
