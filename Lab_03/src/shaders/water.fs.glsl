#version 330 core

in vec2 texCoords;

uniform sampler2D textureSampler;
uniform float time;

out vec4 FragColor;

void main()
{    
    vec2 timeScale = vec2(1.0, 2.0);
    vec2 offsetScale = vec2(2.0, 2.0);
    vec2 waveFunc = vec2(
        cos(time * timeScale.x + (texCoords.x + texCoords.y) * offsetScale.x),
        sin(time * timeScale.y + (texCoords.x + texCoords.y) * offsetScale.y)
    );

    const vec2 waveAmplitude = vec2(0.025, 0.01);
    vec2 waveOffset =  waveFunc * waveAmplitude;

    vec4 texel = texture(textureSampler, texCoords + waveOffset);
    FragColor = vec4(texel.rgb, 1.0f);
}
