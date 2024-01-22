#version 330 core
#extension GL_ARB_separate_shader_objects : enable

in vec3 color;

out vec4 fColor;

void main()
{
	fColor = vec4(color, 1.0);
}