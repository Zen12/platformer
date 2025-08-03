#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D ourTexture;

void main() {
    //FragColor = vec4(texCoord.x,texCoord.y,0,1);
    FragColor = texture(ourTexture, texCoord);
}