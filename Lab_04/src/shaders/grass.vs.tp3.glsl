#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

uniform mat4 mvp;
uniform float time;

void main()
{
    vec3 pos = position;
    pos.x += inTexCoords.y*sin(time/2.0 + pos.x + pos.z)*0.05;
    pos.z += inTexCoords.y*sin(time/2.0 + pos.x + pos.z)*0.05;

    gl_Position = mvp * vec4(pos.x, pos.y, pos.z, 1.0);
    texCoords = inTexCoords;
}
