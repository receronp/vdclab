#version 410

/* 
La siguiente línea no es de GLSL. Antes de compilar el shader, la librería PGUPV la sustituirá por
la definición del bloque de uniforms correspondiente (ver el fichero glMatrices.h)
*/
$GLMatrices

// Dos atributos de entrada:
in vec3 position;
in vec4 vertcolor;

/* Un valor de salida (el color final del vértice, que será interpolado entre los fragmentos 
generados al rasterizar la primitiva.
*/
out vec4 vtxColor;

void main()
{
	// Simplemente copiamos el color de entrada
	vtxColor = vertcolor;
	// Es obligatorio escribir en gl_Position la coordenada en clip space del vértice
	gl_Position = modelviewMatrix * vec4(position,1.0);
}
