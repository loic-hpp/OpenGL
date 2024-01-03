#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position.xyz + normalize(normal)*0.1f, 1.0f);
}
