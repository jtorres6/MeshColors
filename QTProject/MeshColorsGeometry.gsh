#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in int VertexIndex[];
in vec4 geometry_Color[];

flat out int Index[3];
out vec4 fragment_Color;

void main(void)
{
    fragment_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    gl_Position = gl_in[0].gl_Position;
    fragment_Color = geometry_Color[0];
    Index[0] = VertexIndex[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fragment_Color = geometry_Color[1];
    Index[1] = VertexIndex[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    fragment_Color = geometry_Color[2];
    Index[2] = VertexIndex[2];
    EmitVertex();

    EndPrimitive();
}
