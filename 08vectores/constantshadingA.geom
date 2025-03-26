#version 410 core

layout (lines) in;
layout (line_strip) out;
layout (max_vertices = 5) out;

$GLMatrices

in vec4 vtxColor[2];

out vec4 colorI;

void main() {

	colorI = vtxColor[0];
	gl_Position = projMatrix * gl_in[0].gl_Position;
	EmitVertex();

	colorI = vtxColor[1];
	gl_Position = projMatrix * gl_in[1].gl_Position;
	EmitVertex();
	EndPrimitive();

	#define PI 3.1415926538

	// Punta de la flecha
	const float angle = PI/6;
	const mat4 rot = mat4(
		cos(angle), sin(angle), 0.0, 0.0, 
		-sin(angle), cos(angle), 0.0, 0.0, 
		0.0, 0.0, 1.0, 0.0, 
		0.0, 0.0, 0.0, 1.0);
	
	vec4 p = gl_in[0].gl_Position + (gl_in[1].gl_Position - gl_in[0].gl_Position) * 0.6;
	mat4 trans = mat4(
		1.0, 0.0, 0.0, 0.0, 
		0.0, 1.0, 0.0, 0.0, 
		0.0, 0.0, 1.0, 0.0, 
		gl_in[1].gl_Position.x, gl_in[1].gl_Position.y, gl_in[1].gl_Position.z, 1.0); 

	mat4 invtrans = mat4(
		1.0, 0.0, 0.0, 0.0, 
		0.0, 1.0, 0.0, 0.0, 
		0.0, 0.0, 1.0, 0.0, 
		-gl_in[1].gl_Position.x, -gl_in[1].gl_Position.y, -gl_in[1].gl_Position.z, 1.0);  

	colorI = vtxColor[1];
	gl_Position = projMatrix * (trans*rot*invtrans*p);
	EmitVertex();


	colorI = vtxColor[1];
	gl_Position = projMatrix * gl_in[1].gl_Position;
	EmitVertex();

	colorI = vtxColor[1];
	gl_Position = projMatrix * (trans*transpose(rot)*invtrans*p);
	EmitVertex();

}

