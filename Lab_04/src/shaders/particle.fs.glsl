#version 330 core

out vec4 FragColor;

uniform sampler2D textureSampler;

in ATTRIB_GS_OUT {
    vec4 color;
    vec2 texCoords;
} attribIn;

void main() {
    // TODO
    vec4 texelColor = texture(textureSampler, attribIn.texCoords);
    if(texelColor.a < 0.05)
        discard;
    vec4 finalColor = attribIn.color * texelColor;

    FragColor = finalColor;
}
