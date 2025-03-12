#include <random>
#include "buildModel.h"

/**

TODO:

Construye un modelo de PGUPV con una representaci�n de mapa de alturas con la malla dada.
La malla de entrada es de dimensi�n 2, y tiene un atributo de tipo float.
Si las coordenadas de una muestra son (x, y), y su atributo es h, el v�rtice que lo representar�
estar� en:

glm::vec3(x, h, -y)

Si las celdas son tri�ngulos (es decir, la funci�n getCellSamples devuelve 3), se deber� dibujar un tri�ngulo
de OpenGL por celda. Si las celdas son cuadril�teros (es decir, la funci�n getCellSamples devuelve 4), se deber�n dibujar 2
tri�ngulos por celda.

Tambi�n deber�s establecer el color de cada v�rtice. Dise�a tu propio mapa de colores, teniendo en cuenta el rango de
valores de las muestras de entrada.

*/

struct MinMaxSamples {
	size_t minIdx, // Índice de la muestra con valor mínimo
		maxIdx; // Índice de la muestra con el valor máximo
};

/**
Devuelve el índice de las muestras con el valor mínimo y máximo
\param grid la malla
\return un objecto MinMaxCells que contiene el índice de ambas muestras
*/

MinMaxSamples findMinMaxSamples(const vdc::Grid<glm::vec2, float> &grid) {

	// Inserta aquí tu código
	float min = FLT_MAX, max = -FLT_MAX;
	size_t minIdx = 0, maxIdx = 0;

	// Recorre todas las muestras de la malla y busca el valor mínimo y máximo
	for (size_t sampleIdx = 0; sampleIdx < grid.numSamples(); sampleIdx++) {
		float v = grid.getSampleValue(sampleIdx);
		if (v < min) {
			min = v;
			minIdx = sampleIdx;
		}
		if (v > max) {
			max = v;
			maxIdx = sampleIdx;
		}
	}

	return MinMaxSamples{minIdx, maxIdx};
}

std::shared_ptr<PGUPV::Model> vdc::build2DHeightMapFromGrid(const vdc::Grid<glm::vec2, float>& g) {
	auto model = std::make_shared<Model>();

	std::vector<glm::vec3> coordinates;
	std::vector<glm::vec4> colors;
	std::vector<unsigned int> indices;

	// Obtener valores mínimos y máximos para normalizar colores
	MinMaxSamples minMax = findMinMaxSamples(g);
	float minHeight = g.getSampleValue(minMax.minIdx);
	float maxHeight = g.getSampleValue(minMax.maxIdx);

	// Recorrer las muestras para generar los vértices y los colores
	for (size_t i = 0; i < g.numSamples(); i++) {
		glm::vec2 pos = g.getSamplePosition(i);
		float height = g.getSampleValue(i);

		// Convertir a coordenadas 3D
		coordinates.push_back(glm::vec3(pos.x, height, -pos.y));

		// Generar un color basado en el valor de altura (mapa de colores simple)
		float t = (height - minHeight) / (maxHeight - minHeight); // Normalización [0,1]
		colors.push_back(glm::vec4(t, 0.0f, 1.0f - t, 1.0f));	  // Color degradado entre azul y rojo
	}

	// Recorrer las celdas para construir los triángulos
	for (size_t cellIdx = 0; cellIdx < g.numCells(); cellIdx++) {
		size_t cellSamples[4];
		size_t nc = g.getCellSamples(cellIdx, cellSamples);

		// Si es un triángulo, agregar un solo triángulo
		if (nc == 3) {
			indices.push_back(cellSamples[0]);
			indices.push_back(cellSamples[1]);
			indices.push_back(cellSamples[2]);
		}
		// Si es un cuadrilátero, dividir en dos triángulos
		else if (nc == 4) {
			indices.push_back(cellSamples[0]);
			indices.push_back(cellSamples[1]);
			indices.push_back(cellSamples[2]);

			indices.push_back(cellSamples[0]);
			indices.push_back(cellSamples[2]);
			indices.push_back(cellSamples[3]);
		}
	}

	// Malla
	auto base = std::make_shared<Mesh>();
	base->addVertices(coordinates);
	base->addIndices(indices);
	base->addColors(colors);

	base->addDrawCommand(new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));
	model->addMesh(base);
	return model;
}


float secretFunc(float, float);


/**
Dada una malla, asigna a cada muestra su valor (dada su posici�n)
*/
void fillSamples(vdc::Grid<glm::vec2, float>& grid) {
	for (size_t i = 0; i < grid.numSamples(); i++) {
		auto v = grid.getSamplePosition(i);
		grid.setSampleValue(i, secretFunc(v.x, v.y));
	}
}


// Malla uniforme
std::unique_ptr<vdc::UniformGrid<glm::vec2, float>> vdc::buildUniformGrid(const int horizontalVtcs, const int verticalVtcs, const glm::vec2& min, const glm::vec2& max)
{
	auto uniformGrid = std::unique_ptr<vdc::UniformGrid<glm::vec2, float>>(new vdc::UniformGrid<glm::vec2, float>(min, max, { horizontalVtcs, verticalVtcs }));
	fillSamples(*uniformGrid);
	return uniformGrid;
}


/**
TODO:

Cambia en los siguientes m�todos la forma de construir las mallas para que, con el mismo n�mero de muestras
que la malla uniforme, se obtenga una mejor aproximaci�n a la funci�n original.
La versi�n que se entrega define todas las mallas iguales (las muestras est�n equiespaciadas)
*/

// Malla rectil�nea
std::unique_ptr<vdc::RectilinearGrid<glm::vec2, float>>  vdc::buildRectilinearGrid(
    const int horizontalVtcs, const int verticalVtcs, const glm::vec2& min, const glm::vec2& max) {

    std::vector<float> posX, posY;

    for (int i = 0; i < horizontalVtcs; i++) {
        float t = static_cast<float>(i) / (horizontalVtcs - 1);
        posX.push_back(min.x + (max.x - min.x) * (t * t)); // Más denso en un extremo
    }

    for (int i = 0; i < verticalVtcs; i++) {
        float t = static_cast<float>(i) / (verticalVtcs - 1);
        posY.push_back(min.y + (max.y - min.y) * std::sin(t * glm::half_pi<float>())); // Más denso en el centro
    }

	auto rectilinearGrid = std::unique_ptr<vdc::RectilinearGrid<glm::vec2, float>>(new vdc::RectilinearGrid<glm::vec2, float>(posX, posY));
	fillSamples(*rectilinearGrid);
	return rectilinearGrid;
}

// Malla estructurada
std::unique_ptr<vdc::StructuredGrid<glm::vec2, float>> vdc::buildStructuredGrid(
	const int horizontalVtcs, const int verticalVtcs, const glm::vec2 &min, const glm::vec2 &max) {

	auto structuredGrid = std::unique_ptr < vdc::StructuredGrid<glm::vec2, float>>(new vdc::StructuredGrid<glm::vec2, float>({ horizontalVtcs, verticalVtcs }));
	size_t i = 0;

	auto offset = (max - min) / glm::vec2{ horizontalVtcs - 1, verticalVtcs - 1 };
	std::default_random_engine generator;
	std::uniform_real_distribution<float> jitter(-0.1f, 0.1f); // Perturbación pequeña

	for (int y = 0; y < verticalVtcs; y++) {
		for (int x = 0; x < horizontalVtcs; x++) {
			glm::vec2 pos = min + glm::vec2{ x, y } *offset;

			// Perturbación aleatoria dentro de un rango
			pos.x += jitter(generator) * offset.x;
			pos.y += jitter(generator) * offset.y;

			structuredGrid->setSamplePosition(i++, pos);
		}
	}
	fillSamples(*structuredGrid);
	return structuredGrid;
}

// Malla no estructurada
std::unique_ptr<vdc::UnstructuredTriangleGrid<glm::vec2, float>>
vdc::buildUnstructuredGrid(const int horizontalVtcs, const int verticalVtcs,
                           const glm::vec2& min, const glm::vec2& max) {

	auto unstructuredGrid = std::unique_ptr < vdc::UnstructuredTriangleGrid<glm::vec2, float>>(
		new vdc::UnstructuredTriangleGrid<glm::vec2, float>(
			{ static_cast<size_t>(horizontalVtcs) * verticalVtcs, 2ULL * (horizontalVtcs - 1) * (verticalVtcs - 1) }));
						
    std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<float> jitter(-0.1f, 0.1f);

    size_t i = 0;
    auto offset = (max - min) / glm::vec2{ horizontalVtcs - 1, verticalVtcs - 1 };

    // Generar posiciones con variación solo en puntos internos
    for (int y = 0; y < verticalVtcs; y++) {
        for (int x = 0; x < horizontalVtcs; x++) {
            glm::vec2 pos = min + glm::vec2{x, y} * offset;

            // Agregar ruido solo si no es un punto de borde
            if (x > 0 && x < horizontalVtcs - 1 && y > 0 && y < verticalVtcs - 1) {
                pos.x += jitter(generator) * offset.x;
                pos.y += jitter(generator) * offset.y;
            }

            unstructuredGrid->setSamplePosition(i++, pos);
        }
    }

    fillSamples(*unstructuredGrid);

    // Conectar los puntos en un patrón Zig-Zag
    i = 0;
    for (int cellY = 0; cellY < verticalVtcs - 1; cellY++) {
        for (int cellX = 0; cellX < horizontalVtcs - 1; cellX++) {
            size_t v0 = cellX + cellY * horizontalVtcs;
            size_t v1 = v0 + 1;
            size_t v2 = v0 + horizontalVtcs;
            size_t v3 = v2 + 1;

            // Alternar triangulación para mejorar la representación
			size_t v[3];
            if ((cellX + cellY) % 2 == 0) {
				v[0] = v0;
				v[1] = v1;
				v[2] = v3;	
                unstructuredGrid->setCell(i++, v);

				v[0] = v0;
				v[1] = v3;
				v[2] = v2;	
                unstructuredGrid->setCell(i++, v);
            } else {
				v[0] = v1;
				v[1] = v3;
				v[2] = v2;	
                unstructuredGrid->setCell(i++, v);

				v[0] = v1;
				v[1] = v2;
				v[2] = v0;	
                unstructuredGrid->setCell(i++, v);
            }
        }
    }
	assert(i == (horizontalVtcs - 1) * (verticalVtcs - 1) * 2);
    return unstructuredGrid;
}

/**
TODO:

Tendr�s que implementar el algoritmo de derivaci�n visto en clase.

*/

std::shared_ptr<vdc::StructuredGrid<glm::vec2, glm::vec2>>
vdc::derivative(const vdc::UniformGrid<glm::vec2, float> &g) {
	// Obtener dimensiones de la malla
	int nx = g.getNumSamplesPerDimension(0);
	int ny = g.getNumSamplesPerDimension(1);
	auto result = std::make_shared<vdc::StructuredGrid<glm::vec2, glm::vec2>>(std::vector<int>{nx, ny});

	// Obtener tamaño de celda en cada dirección
	float dx = (g.getSamplePosition(1).x - g.getSamplePosition(0).x);
	float dy = (g.getSamplePosition(nx).y - g.getSamplePosition(0).y);

	// Calcular derivadas parciales
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			int i = x + y * nx; // Índice lineal

			float dfdx = 0.0f, dfdy = 0.0f;

			// Derivada en X
			if (x > 0 && x < nx - 1) {
				dfdx = (g.getSampleValue(i + 1) - g.getSampleValue(i - 1)) / (2 * dx); // Diferencias centrales
			} else if (x == 0) {
				dfdx = (g.getSampleValue(i + 1) - g.getSampleValue(i)) / dx; // Diferencias hacia adelante
			} else {
				dfdx = (g.getSampleValue(i) - g.getSampleValue(i - 1)) / dx; // Diferencias hacia atrás
			}

			// Derivada en Y
			if (y > 0 && y < ny - 1) {
				dfdy = (g.getSampleValue(i + nx) - g.getSampleValue(i - nx)) / (2 * dy); // Diferencias centrales
			} else if (y == 0) {
				dfdy = (g.getSampleValue(i + nx) - g.getSampleValue(i)) / dy; // Diferencias hacia adelante
			} else {
				dfdy = (g.getSampleValue(i) - g.getSampleValue(i - nx)) / dy; // Diferencias hacia atrás
			}

			// Asignar posición y valor de la derivada
			result->setSamplePosition(i, g.getSamplePosition(i));
			result->setSampleValue(i, glm::vec2(dfdx, dfdy));
		}
	}

	return result;
}
