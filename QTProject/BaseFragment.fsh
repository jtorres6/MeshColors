#version 450 core
in vec4 next_color;
void main(void)
{
    gl_FragColor = next_color;
}
