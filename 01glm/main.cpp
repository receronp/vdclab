/*

Ejemplo de uso de la librería glm.

La biblioteca glm proporciona una implementación en C++ de las funciones
matemáticas disponibles en el lenguaje de programación de shaders de OpenGL
(GLSL).

En este proyecto se mostrará cómo utilizar las funciones más comunes.

Tienes la documentación completa de la librería en:

<ruta-de-las-practicas>/librerias/glm/doc/manual.pdf

o bien online en:

https://glm.g-truc.net

*/

/*
El primer (y único paso para utilizar la librería) consiste en importar la cabecera:
*/

#include <glm/glm.hpp>
#include <glm/gtc/vec1.hpp>


#include <iostream> // Para std::out
#include "util.h"

int main() {
	// Los tipos de datos principales que ofrece la librería son:
	// Vectores:
	// glm::vec1  una coordenada, igual que un escalar
	// glm::vec2  dos coordenadas
	// glm::vec3  tres
	// glm::vec4  cuatro

	// Matrices:
	//   Cuadradas: glm::mat2, glm::mat3, glm::mat4
	//   Rectangulares: glm::mat3x2  matriz rectangular de 3 filas por 2 columnas

	// ¡CUIDADO! Siempre hay que inicializar los tipos de glm a un valor (no se inicializan a cero)

	std::cout << "Constructores:\n-----------------\n";
	glm::vec2 v2{ 1.0f, 1.0f };
	std::cout << "glm::vec2 v2{ 1.0f, 1.0f }; -> " << to_string(v2) << "\n";

	glm::vec3 ott{ 1.0f, 2.0f, 3.0f };
	std::cout << "glm::vec3 ott{1.0f, 2.0f, 3.0f}; -> " << to_string(ott) << "\n";

	glm::vec4 v4z{ 0.0f };
	std::cout << "glm::vec4 v4z{0.0f}; -> " << to_string(v4z) << "\n";

	glm::vec4 v4{ ott, 1.0f };  // cuatro
	std::cout << "glm::vec4 v4{ ott, 1.0f }; -> " << to_string(v4) << "\n";

	std::cout << "\nOperaciones sobre vectores:\n--------------------------------\n";

	std::cout << "Sumas, restas\n";
	glm::vec2 onetwo{ 1.0f, 2.0f };
	std::cout << to_string(v2) << " + " << to_string(onetwo) << " = " << to_string(v2 + onetwo) << "\n";

	std::cout << "con escalares también funciona:\n";
	std::cout << to_string(v2) << " + 2.0 = " << to_string(v2 + 2.0f) << "\n";

	std::cout << "Producto (componente a componente)\n";
	v2 = { 3.0f, 3.0f };
	std::cout << to_string(v2) << " * " << to_string(onetwo) << " = " << to_string(v2 * onetwo) << "\n";

	std::cout << "Producto escalar\n";
	std::cout << "glm::dot(" << to_string(v2) << ", " << to_string(onetwo) << ") = " << glm::dot(v2, onetwo) << "\n";

	std::cout << "Producto vectorial\n";
	glm::vec3 v3{ 1.0f, 0.0f, 0.0f };
	glm::vec3 zoz{ 0.0f, 1.0f, 0.0f };
	std::cout << "glm::cross(" << to_string(v3) << ", " << to_string(zoz) << ") = " << to_string(glm::cross(v3, zoz)) << "\n";

	std::cout << "Magnitud de un vector (módulo)\n";
	std::cout << "glm::length(" << to_string(v2) << ") = " << glm::length(v2) << "\n";

	std::cout << "Vector normalizado:\n";
	std::cout << "glm::normalize(v2) -> " << to_string(glm::normalize(v2)) << "\n";

	std::cout << "Distancia entre dos puntos:\n";
	std::cout << "glm::distance(" << to_string(v3) << ", " << to_string(zoz) << ") -> " << glm::distance(v3, zoz) << "\n";

	std::cout << "\n\ny muchas otras funciones para trabajar con vectores y matrices...\n";
}

