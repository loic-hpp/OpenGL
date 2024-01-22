#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

uniform mat4 mvp;

out vec3 color;

void  main()
{
	color = vColor;
	gl_Position = mvp * vec4(vPosition, 1.0);
}