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
std::unique_ptr<vdc::RectilinearGrid<glm::vec2, float>>  vdc::buildRectilinearGrid(const int horizontalVtcs, const int verticalVtcs, const glm::vec2& min, const glm::vec2& max) {
	std::vector<float> posX;
	for (int i = 0; i < horizontalVtcs; i++) {
		posX.push_back((max.x - min.x) * i / (horizontalVtcs - 1) + min.x);
	}
	std::vector<float> posY;
	for (int i = 0; i < verticalVtcs; i++) {
		posY.push_back((max.y - min.y) * i / (verticalVtcs - 1) + min.y);
	}
	auto rectilinearGrid = std::unique_ptr<vdc::RectilinearGrid<glm::vec2, float>>(new vdc::RectilinearGrid<glm::vec2, float>(posX, posY));
	fillSamples(*rectilinearGrid);
	return rectilinearGrid;
}

// Malla estructurada
std::unique_ptr<vdc::StructuredGrid<glm::vec2, float>> vdc::buildStructuredGrid(const int horizontalVtcs, const int verticalVtcs, const glm::vec2& min, const glm::vec2& max) {
	auto structuredGrid = std::unique_ptr < vdc::StructuredGrid<glm::vec2, float>>(new vdc::StructuredGrid<glm::vec2, float>({ horizontalVtcs, verticalVtcs }));
	size_t i = 0;
	auto offset = (max - min) / glm::vec2{ horizontalVtcs - 1, verticalVtcs - 1 };
	for (int y = 0; y < verticalVtcs; y++) {
		for (int x = 0; x < horizontalVtcs; x++) {
			glm::vec2 pos = min + glm::vec2{ x, y } *offset;
			structuredGrid->setSamplePosition(i++, pos);
		}
	}
	fillSamples(*structuredGrid);
	return structuredGrid;
}

// Malla no estructurada
std::unique_ptr<vdc::UnstructuredTriangleGrid<glm::vec2, float>> vdc::buildUnstructuredGrid(const int horizontalVtcs, const int verticalVtcs, const glm::vec2& min, const glm::vec2& max) {
	auto unstructuredGrid = std::unique_ptr < vdc::UnstructuredTriangleGrid<glm::vec2, float>>(
		new vdc::UnstructuredTriangleGrid<glm::vec2, float>(
			{ static_cast<size_t>(horizontalVtcs) * verticalVtcs, 2ULL * (horizontalVtcs - 1) * (verticalVtcs - 1) }));
	size_t i = 0;

	auto offset = (max - min) / glm::vec2{ horizontalVtcs - 1, verticalVtcs - 1 };
	for (int y = 0; y < verticalVtcs; y++) {
		for (int x = 0; x < horizontalVtcs; x++) {
			glm::vec2 pos = min + glm::vec2{ x, y } *offset;
			unstructuredGrid->setSamplePosition(i++, pos);
		}
	}

	fillSamples(*unstructuredGrid);
	assert(i == horizontalVtcs * verticalVtcs);

	i = 0;
	for (int cellY = 0; cellY < verticalVtcs - 1; cellY++) {
		for (int cellX = 0; cellX < horizontalVtcs - 1; cellX++) {
			size_t v[3];
			size_t firstV = cellX + horizontalVtcs * cellY;
			v[0] = firstV;
			v[1] = firstV + 1;
			v[2] = v[1] + horizontalVtcs;
			unstructuredGrid->setCell(i++, v);

			v[0] = firstV;
			v[1] = firstV + 1 + horizontalVtcs;
			v[2] = v[1] - 1;
			unstructuredGrid->setCell(i++, v);
		}
	}
	assert(i == (horizontalVtcs - 1) * (verticalVtcs - 1) * 2);
	return unstructuredGrid;
}

/**
TODO:

Tendr�s que implementar el algoritmo de derivaci�n visto en clase.

*/

std::shared_ptr<vdc::StructuredGrid<glm::vec2, glm::vec2>> vdc::derivative(const vdc::UniformGrid<glm::vec2, float>& g) {
	std::vector<int> dims{ g.getNumSamplesPerDimension(0), g.getNumSamplesPerDimension(1) };
	auto result = std::make_shared<vdc::StructuredGrid<glm::vec2, glm::vec2>>(dims);

	for (size_t i = 0; i < g.numSamples(); i++) {
		result->setSamplePosition(i, g.getSamplePosition(i));
		result->setSampleValue(i, glm::vec2(1, 1));
	}
	return result;
}
