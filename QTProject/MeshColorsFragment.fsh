#version 450 core
uniform highp vec3 points[1024];
layout(std430, binding = 3) buffer MeshColorsData
{
    int Resolution;
    vec4 Colors[132][132];
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
    vec4 B =  floor(Resolution * color);
    vec4 w = (Resolution * color) - B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    if(ColorLerpEnabled)
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[int(B[0])][int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            vec4 c1 = w.r*Colors[int(B.r+1)][int(B.g)];
            vec4 c2 = w.g*Colors[int(B.r)][int(B.g+1)];
            vec4 c3 = w.b*Colors[int(B.r)][int(B.g)];
            c = c1+c2+c3;
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            vec4 c1 = (1-w.r)*Colors[int(B.r)][int(B.g+1)];
            vec4 c2 = (1-w.g)*Colors[int(B.r+1)][int(B.g)];
            vec4 c3 = (1-w.b)*Colors[int(B.r+1)][int(B.g+1)];
            c =  c1+c2+c3;
        }
    }
    else
    {
        if(round(w.r+w.g+w.b) == 0)
        {
           c = Colors[int(B[0])][int(B[1])];
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            float maxc = max(max(w.r, w.g), w.b);

            if(maxc == w.r)
            {
                c = Colors[int(B.r+1)][int(B.g)];
            }
            else if(maxc == w.g)
            {
                c = Colors[int(B.r)][int(B.g+1)];
            }
            else if(maxc == w.b)
            {
                c = Colors[int(B.r)][int(B.g)];
            }
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            float maxc = max(max((1-w.r), (1-w.g)), (1-w.b));

            if(maxc == (1-w.r))
            {
                c = Colors[int(B.r)][int(B.g+1)];
            }
            else if(maxc == (1-w.g))
            {
                c = Colors[int(B.r+1)][int(B.g)];
            }
            else if(maxc == (1-w.b))
            {
                c = Colors[int(B.r+1)][int(B.g+1)];
            }
        }
    }

    gl_FragColor = c;
}
