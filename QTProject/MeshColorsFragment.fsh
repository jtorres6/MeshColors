#version 450 core
uniform highp vec3 points[1024];
layout(std430, binding = 3) buffer MeshColorsData
{
    int Resolution[132];
    vec4 Colors[132][256];
};

const int R = 4;

flat in int Index[3];
flat in int res;
in vec4 out_fragFaceIndexes;
in vec4 next_color;
out vec4 frag_color;

uniform bool ColorLerpEnabled;

void main(void)
{
    vec4 color = next_color;
    vec4 B =  floor(Resolution[Index[0]] * color);
    vec4 w = (Resolution[Index[0]] * color) - B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    if(ColorLerpEnabled)
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[Index[0]][int(B[0]) * Resolution[Index[0]] + int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            vec4 c1 = w.r*Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g)];
            vec4 c2 = w.g*Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g+1)];
            vec4 c3 = w.b*Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g)];
            c = c1+c2+c3;
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            vec4 c1 = (1-w.r)*Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g+1)];
            vec4 c2 = (1-w.g)*Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g)];
            vec4 c3 = (1-w.b)*Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g+1)];
            c =  c1+c2+c3;
        }
    }
    else
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[Index[0]][int(B[0]) * Resolution[Index[0]] + int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            float maxc = max(max(w.r, w.g), w.b);

            if(maxc == w.r)
            {
                c = Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g)];
            }
            else if(maxc == w.g)
            {
                c = Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g+1)];
            }
            else if(maxc == w.b)
            {
                c = Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g)];
            }
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            float maxc = max(max((1-w.r), (1-w.g)), (1-w.b));

            if(maxc == (1-w.r))
            {
                c = Colors[Index[0]][int(B.r) * Resolution[Index[0]] + int(B.g+1)];
            }
            else if(maxc == (1-w.g))
            {
                c = Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g)];
            }
            else if(maxc == (1-w.b))
            {
                c = Colors[Index[0]][int(B.r+1) * Resolution[Index[0]] + int(B.g+1)];
            }
        }
    }

    {
        gl_FragColor = c;
    }
}
