#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Vertex;

void main(void)
{
 gl_Position = vec4(Vertex, 1.0);
}