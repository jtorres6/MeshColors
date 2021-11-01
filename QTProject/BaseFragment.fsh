#version 420

in vec4 next_color;
out vec4 out_Color;

void main(void)
{
    out_Color = next_color;
}
