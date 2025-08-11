#version 330 core
out vec4 FragColor;


void main()
{
    //float dist = distance(localPosition, vec3(0,0,0));
    //float transperent = 1.0 - (dist * 0.09);
    //transperent = clamp(transperent, 0.01, 0.30) + 0.01; // a bit of light for far-far distance
    FragColor = vec4(1, 1, 0, 0.2);
}