#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in int VertexIndex[];
in vec4 out_color[];

flat out int Index[];
out vec4 next_color;

void main(void)
{
    next_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    gl_Position = gl_in[0].gl_Position;
    next_color = out_color[0];
    Index[0] = VertexIndex[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    next_color = out_color[1];
    Index[1] = VertexIndex[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    next_color = out_color[2];
    Index[2] = VertexIndex[2];
    EmitVertex();

    EndPrimitive();
}
