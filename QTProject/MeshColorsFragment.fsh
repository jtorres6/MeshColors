#version 140
uniform highp int R = 3;
uniform highp vec3[9] points;


varying vec4 color;
void main(void)
{
    vec4 B =  floor(3*color);
    vec4 w =(3*color)-B;
    vec4 c = vec4(0.0f,0.0f,0.0f,0.0f);

    if(round(w.r+w.g+w.b) == 0)
    {
       c = vec4(points[int(round(B[0]+B[1]*3))], 0.0f);
    }
    else if(round(w.r+w.g+w.b) == 1)
    {
        vec3 c1 = w.r*points[int(round((B.r+1)*3+B.g))];
        vec3 c2 = w.g*points[int(round(B.r*3+(B.g+1)))];
        vec3 c3 = w.b*points[int(round(B.r*3+B.g))];
        c = vec4(c1+c2+c3, 1.0f);
    }
    else if(round(w.r+w.g+w.b) == 2)
    {
        vec3 c1 = w.g*points[int(round(B.r*3+(B.g+1)))];
        vec3 c2 = w.r*points[int(round((B.r+1)*3+B.g))];
        vec3 c3 = w.b*points[int(round((B.r+1)*3+(B.g+1)))];
        c =  vec4(c1+c2+c3, 1.0f);
    }
    gl_FragColor = c;
}
