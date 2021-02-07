#version 450 core

in vec4 out_color;

out vec4 frag_color;

void main(void)
{
    frag_color = out_color;
}
