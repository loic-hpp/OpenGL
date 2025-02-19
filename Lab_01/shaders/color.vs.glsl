#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

out vec3 color;

void  main()
{
	color = vColor;
	gl_Position = vec4(vPosition, 1.0);
}