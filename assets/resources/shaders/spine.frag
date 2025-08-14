#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D ourTexture;

void main() {
    //FragColor = vec4(texCoord.x,texCoord.y,0,1);
    vec4 color = texture(ourTexture, texCoord);
    //vec3 c = color.xyz * 0.9f;
    //color.a = 0.5f; // half as bright
    FragColor = color;
}