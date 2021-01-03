#version 140
uniform highp vec3[256] points;
uniform mediump int R = 4;
uniform lowp bool meshColorsEnabled = false;

varying vec4 color;
void main(void)
{
    if(meshColorsEnabled)
    {
        vec4 B =  floor(R*color);
        vec4 w =(R*color)-B;
        vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

        if(round(w.r+w.g+w.b) == 0)
        {
           c = vec4(points[int(round(B[0]+B[1]*R))], 0.0f);
        }
        else if(round(w.r+w.g+w.b) == 1)
        {
            vec3 c1 = w.r*points[int(round((B.r+1)*R+B.g))];
            vec3 c2 = w.g*points[int(round(B.r*R+(B.g+1)))];
            vec3 c3 = w.b*points[int(round(B.r*R+B.g))];
            c = vec4(c1+c2+c3, 1.0f);
        }
        else if(round(w.r+w.g+w.b) == 2)
        {
            vec3 c1 = (1-w.r)*points[int(round(B.r*R+(B.g+1)))];
            vec3 c2 = (1-w.g)*points[int(round((B.r+1)*R+B.g))];
            vec3 c3 = (1-w.b)*points[int(round((B.r+1)*R+(B.g+1)))];
            c =  vec4(c1+c2+c3, 1.0f);
        }
        gl_FragColor = c;
    }
    else
    {
        gl_FragColor = color;
    }
}
