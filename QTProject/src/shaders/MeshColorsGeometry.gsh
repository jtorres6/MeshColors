#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in int VertexIndex[];
in vec3 VertexNormal[];
in vec4 geometry_Color[];

flat out int Index[3];
flat out vec3 Normal[3];
out vec4 fragment_Color;
out vec4 fragPos;

void main(void)
{
    fragment_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    fragPos = gl_Position = gl_in[0].gl_Position;
    fragment_Color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    Index[0] = VertexIndex[0];
    Normal[0] = VertexNormal[0];
    EmitVertex();

    fragPos = gl_Position = gl_in[1].gl_Position;
    fragment_Color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    Index[1] = VertexIndex[1];
    Normal[1] = VertexNormal[1];
    EmitVertex();

    fragPos = gl_Position = gl_in[2].gl_Position;
    fragment_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Index[2] = VertexIndex[2];
    Normal[2] = VertexNormal[2];
    EmitVertex();

    EndPrimitive();
}
