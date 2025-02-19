#version 410 core

$GLMatrices

uniform vec4 lightpos; // lightpos (in eye space)
uniform vec4 diffuseColor = vec4(0.7, 0.5, 0.5, 1.0);
uniform vec4 ambientColor = vec4(0.0);
uniform bool useVertexColor;

flat in vec3 normal;
in vec4 position_eyeI; // interpolated vertexPos (in eye space)
in vec4 colorI;

out vec4 final_color;

void main()
{
    if (useVertexColor) {
        final_color = colorI;
    } else {
	    vec3 lightDir = normalize(lightpos.xyz-position_eyeI.xyz);
	    float intensity = max(dot(lightDir,normal), 0.0);
	
	    final_color = (diffuseColor * intensity) + ambientColor;
    }
}
