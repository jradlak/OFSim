#version 330 core
layout (location = 0) in vec3 aPos;

out float vFragDepth;

uniform highp mat4 model;
uniform highp mat4 transformation;

void main()
{
	gl_Position = transformation * vec4(aPos, 1.0);
	
	vFragDepth = 1.0 + gl_Position.w;
}