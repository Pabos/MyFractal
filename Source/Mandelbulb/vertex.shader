
#version 440 core
#define DEBUG

layout(location = 3) in vec4 vertexpos;
layout(location = 4) in vec4 colorcode;
uniform mat4 MVPfrac;
uniform float alfa = 1;
uniform vec3 displace;

out vec4 colorout;
void main()
{
	gl_Position	= MVPfrac * vec4(vertexpos.xyz, 1);
	colorout	= vec4(colorcode.xyz, alfa);
}