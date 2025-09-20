#version 300 es
precision mediump float;

in vec2 TexCoords;           // from vertex shader
uniform sampler2D ourTexture;
uniform float fillAmount;
uniform vec4 tint;

out vec4 FragColor;          // WebGL2 output

void main()
{
    if (TexCoords.x > fillAmount)
        discard;
    vec4 texColor = texture(ourTexture, TexCoords) * tint;
    FragColor = texColor;
}
