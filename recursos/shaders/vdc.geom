#version 410 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

$GLMatrices

in vec4 vtxColor[3];

flat out vec3 normal;
out vec4 position_eyeI;
out vec4 colorI;

void main() {
	vec3 facenormal = normalize(cross(
		gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz,
		gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz));

	for (int i = 0; i < gl_in.length(); i++) {
		normal = facenormal;
		position_eyeI = gl_in[i].gl_Position;
	    colorI = vtxColor[i];
		gl_Position =  projMatrix * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}

