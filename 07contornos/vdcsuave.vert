#version 410 core

$GLMatrices

in vec3 position;
in float texcoord;

out float vtxTexcoord;

void main()
{
	vtxTexcoord = texcoord;
	gl_Position = modelviewprojMatrix * vec4(position,1.0);
}
