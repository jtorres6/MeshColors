#version 450 core
layout(std430, binding = 3) buffer MeshColorsData
{
    int Resolution[132];
    vec4 Colors[132][256];
};

uniform bool BaseRendering;
uniform bool ColorLerpEnabled;

flat in int Index[3];
in vec4 next_color;

void main(void)
{
    vec4 color = next_color;
    if(!BaseRendering)
    {
        const int SampleIndex = Index[0];
        int R = Resolution[SampleIndex];
        vec4 B =  floor(R * color);
        vec4 w = (R * color) - B;
        vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

        if(ColorLerpEnabled)
        {
            if(round(w.r+w.g+w.b) == 0)
            {
               c = Colors[SampleIndex][int(B[0]) * R + int(B[1])];
            }
            else if(round(w.r+w.g+w.b) == 1)
            {
                vec4 c1 = w.r * Colors[SampleIndex][int(B.r+1) * R + int(B.g)];
                vec4 c2 = w.g * Colors[SampleIndex][int(B.r) * R + int(B.g+1)];
                vec4 c3 = w.b * Colors[SampleIndex][int(B.r) * R + int(B.g)];
                c = c1+c2+c3;
            }
            else if(round(w.r+w.g+w.b) == 2)
            {
                vec4 c1 = (1-w.r)*Colors[SampleIndex][int(B.r) * R + int(B.g+1)];
                vec4 c2 = (1-w.g)*Colors[SampleIndex][int(B.r+1) * R + int(B.g)];
                vec4 c3 = (1-w.b)*Colors[SampleIndex][int(B.r+1) * R + int(B.g+1)];
                c =  c1+c2+c3;
            }
        }
        else
        {
            if(round(w.r+w.g+w.b) == 0)
            {
               c = Colors[SampleIndex][int(B[0]) * R + int(B[1])];
            }
            else if(round(w.r+w.g+w.b) == 1)
            {
                float maxc = max(max(w.r, w.g), w.b);

                if(maxc == w.r)
                {
                    c = Colors[SampleIndex][int(B.r+1) * R + int(B.g)];
                }
                else if(maxc == w.g)
                {
                    c = Colors[SampleIndex][int(B.r) * R + int(B.g+1)];
                }
                else if(maxc == w.b)
                {
                    c = Colors[SampleIndex][int(B.r) * R + int(B.g)];
                }
            }
            else if(round(w.r+w.g+w.b) == 2)
            {
                float maxc = max(max((1-w.r), (1-w.g)), (1-w.b));

                if(maxc == (1-w.r))
                {
                    c = Colors[SampleIndex][int(B.r) * R + int(B.g+1)];
                }
                else if(maxc == (1-w.g))
                {
                    c = Colors[SampleIndex][int(B.r+1) * R + int(B.g)];
                }
                else if(maxc == (1-w.b))
                {
                    c = Colors[SampleIndex][int(B.r+1) * R + int(B.g+1)];
                }
            }
        }
        gl_FragColor = c;
    }
    else
    {
        gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}
