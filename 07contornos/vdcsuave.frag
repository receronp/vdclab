#version 410 core

$GLMatrices

in float vtxTexcoord;
out vec4 final_color;

void main()
{
	final_color = vec4(0.0, 0.0, 1.0, 1.0) * (1.0-vtxTexcoord) + vec4(1.0, 1.0, 0.0, 1.0) * vtxTexcoord;
}
