#version 430

layout(std430, binding = 3) buffer MeshColorsData
{
    int Resolution[9000];
    vec4 Colors[9000][289];
};

uniform bool ColorLerpEnabled;
uniform bool LightingEnabled;
uniform vec3 LightPos;

flat in vec3 Normal[3];
in vec4 fragment_Color;

out vec4 out_Color;
in vec4 fragPos;

void main(void)
{
    vec4 color = fragment_Color;

    const int SampleIndex = gl_PrimitiveID;

    const int ArraySize = 17;

    int R = Resolution[SampleIndex];
    vec4 B =  floor(R * color);
    vec4 w = (R * color) - B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    if(ColorLerpEnabled)
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[SampleIndex][int(B[0]) + ArraySize * int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            vec4 c1 = w.r * Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g)];
            vec4 c2 = w.g * Colors[SampleIndex][int(B.r) + ArraySize * int(B.g+1)];
            vec4 c3 = w.b * Colors[SampleIndex][int(B.r) + ArraySize * int(B.g)];
            c = c1+c2+c3;
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            vec4 c1 = (1-w.r)*Colors[SampleIndex][int(B.r) + ArraySize * int(B.g+1)];
            vec4 c2 = (1-w.g)*Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g)];
            vec4 c3 = (1-w.b)*Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g+1)];
            c =  c1+c2+c3;
        }
    }
    else
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[SampleIndex][int(B[0]) + ArraySize * int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            float maxc = max(max(w.r, w.g), w.b);

            if(maxc == w.r)
            {
                c = Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g)];
            }
            else if(maxc == w.g)
            {
                c = Colors[SampleIndex][int(B.r) + ArraySize * int(B.g+1)];
            }
            else if(maxc == w.b)
            {
                c = Colors[SampleIndex][int(B.r) + ArraySize * int(B.g)];
            }
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            float maxc = max(max((1-w.r), (1-w.g)), (1-w.b));

            if(maxc == (1-w.r))
            {
                c = Colors[SampleIndex][int(B.r) + ArraySize * int(B.g+1)];
            }
            else if(maxc == (1-w.g))
            {
                c = Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g)];
            }
            else if(maxc == (1-w.b))
            {
                c = Colors[SampleIndex][int(B.r+1) + ArraySize * int(B.g+1)];
            }
        }
    }

    if(LightingEnabled)
    {
        vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
        float diff = max(dot(normalize(Normal[0]), normalize(LightPos)), 0.0);
        vec3 diffuse = diff * LightColor;

        out_Color = vec4(diffuse * c.xyz, 1.0f);
    }
    else
    {
        out_Color = c;
    }
}
