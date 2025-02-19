#version 410 core

$GLMatrices

in vec3 position;
in vec4 color;

out vec4 vtxColor;

void main()
{
	vtxColor = color;
	gl_Position = modelviewMatrix * vec4(position,1.0);
}
