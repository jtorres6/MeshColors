#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in vec3 VertexNormal[];
in vec4 geometry_Color[];

flat out int Index[3];
flat out vec3 Normal[3];
out vec4 fragment_Color;
out vec4 fragPos;

void main(void)
{
    fragment_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    gl_PrimitiveID = gl_PrimitiveIDIn;

    fragPos = gl_Position = gl_in[0].gl_Position;
    fragment_Color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    Normal[0] = VertexNormal[0];
    EmitVertex();

    fragPos = gl_Position = gl_in[1].gl_Position;
    fragment_Color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    Normal[1] = VertexNormal[1];
    EmitVertex();

    fragPos = gl_Position = gl_in[2].gl_Position;
    fragment_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Normal[2] = VertexNormal[2];
    EmitVertex();

    EndPrimitive();
}
