#version 410 core

$GLMatrices

uniform sampler1D colorMapTextureUnit;

in float vtxTexcoord;
out vec4 final_color;

void main()
{
	final_color = texture(colorMapTextureUnit, vtxTexcoord);
}
