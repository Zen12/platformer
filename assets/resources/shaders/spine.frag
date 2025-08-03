#version 330 core
in vec4 lightColor;
in vec4 darkColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D uTexture;
void main() {
    vec4 texColor = texture(uTexture, texCoord);
    float alpha = texColor.a * lightColor.a;
    fragColor.a = alpha;
    fragColor.rgb = ((texColor.a - 1.0) * darkColor.a + 1.0 - texColor.rgb) * darkColor.rgb + texColor.rgb * lightColor.rgb;
}