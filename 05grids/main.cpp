
#include <iostream>

#include <uniformGrid.h>
#include <rectilinearGrid.h>
#include <structuredGrid.h>
#include <unstructuredTriangleGrid.h>

#include <utils.h>

#define _USE_MATH_DEFINES
#include <math.h>

/*

El objetivo de esta práctica es que aprendas a utilizar las estructuras de datos que representan
los diferentes tipos de malla.

*/

/*

TODO: Completa la función findMinMaxSamples para que, dada una malla, devuelva el índice de
  la muestra con el valor mínimo y el índice de la muestra con el valor máximo.

  Usa la siguiente estructura para devolver dichos valores

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


/*

TODO
Completa la función findMaxRangeCell para devolver la celda de la malla dada que
contiene las muestras con una diferencia entre el valor de sus muestras más grande.

*/

/**
Encuentra la celda con el rango máximo en el valor de sus muestras
\param grid la malla
\return el índice de la celda cuyos valores tienen un rango mayor dentro de la malla
*/

size_t findMaxRangeCell(const vdc::Grid<glm::vec2, float> &grid) {

	// Inserta aquí tu código
	size_t maxRangeCellIdx = 0;
	float maxRange = 0;

	// Recorre todas las celdas de la malla y calcula el rango de valores de sus muestras
	for (size_t cellIdx = 0; cellIdx < grid.numCells(); cellIdx++) {
		size_t cells[4];
		size_t nc = grid.getCellSamples(cellIdx, cells);

		float minV = FLT_MAX, maxV = -FLT_MAX;
		minV = maxV = grid.getSampleValue(cells[0]);
		for (size_t j = 0; j < nc; j++) {
			float v = grid.getSampleValue(cells[j]);
			if (v < minV) minV = v;
			else if (v > maxV) maxV = v;
		}

		float range = maxV - minV;
		if (range > maxRange) {
			maxRange = range;
			maxRangeCellIdx = cellIdx;
		}
	}

	return maxRangeCellIdx;
}


/**
Dada una malla, asigna a cada muestra su valor (dada su posición)
*/
void fillSamples(vdc::Grid<glm::vec2, float> &grid) {
	for (size_t i = 0; i < grid.numSamples(); i++) {
		auto v = grid.getSamplePosition(i);
		grid.setSampleValue(i, v.x*exp(-(v.x*v.x + v.y*v.y)));
	}
}


/*
Muestra los resultados por pantalla
*/
void reportResults(const vdc::Grid<glm::vec2, float> &grid, const MinMaxSamples &mm, const size_t maxRangeCell) {
	std::cout << "Min value: " << grid.getSampleValue(mm.minIdx) <<
		". Sample index: " << mm.minIdx << " at " << PGUPV::to_string(grid.getSamplePosition(mm.minIdx)) << "\n"
		<< "Max value: " << grid.getSampleValue(mm.maxIdx) <<
		". Sample index: " << mm.maxIdx << " at " << PGUPV::to_string(grid.getSamplePosition(mm.maxIdx)) << "\n";

	size_t cells[4];
	size_t nc = grid.getCellSamples(maxRangeCell, cells);

	std::cout << "Cell with maximum range:" << maxRangeCell << "\n"
		<< "Cell corners:\n";
	float minV = FLT_MAX, maxV = -FLT_MAX;
	minV = maxV = grid.getSampleValue(cells[0]);
	for (size_t i = 0; i < nc; i++) {
		float v = grid.getSampleValue(cells[i]);
		std::cout << "  [" << i << "]: " << PGUPV::to_string(grid.getSamplePosition(cells[i])) << "  = " << v << "\n";
		if (v < minV) minV = v;
		else if (v > maxV) maxV = v;
	}

	std::cout << "Range: " << maxV - minV << "\n";
}


/*
Construye una malla no estructurada triangular de ejemplo
*/
void buildTriangularGrid(vdc::UnstructuredTriangleGrid<glm::vec2, float> &grid) {
	float cellW = 4.0f / 50, cellH = 4.0f / 50;
	glm::vec2 llcorner{ -2.f, -2.f };

	size_t cellIdx = 0;
	for (int y = 0; y < 51; y++) {
		for (int x = 0; x < 51; x++) {
			glm::vec2 c = llcorner + glm::vec2(x*cellW, y*cellH);
			grid.setSamplePosition(cellIdx, c);
			cellIdx++;
		}
	}

	fillSamples(grid);

	size_t vertexIndices[3];
	size_t cellId = 0;
	for (int y = 0; y < 50; y++) {
		for (int x = 0; x < 50; x++) {
			size_t origin = y * 51 + x;

			vertexIndices[0] = origin;
			vertexIndices[1] = origin + 51 + 1;
			vertexIndices[2] = origin + 51;
			grid.setCell(cellId++, vertexIndices);

			vertexIndices[0] = origin;
			vertexIndices[1] = origin + 1;
			vertexIndices[2] = origin + 1 + 51;
			grid.setCell(cellId++, vertexIndices);
		}
	}
}



int main() {
	// uniformScalar es una malla uniforme, que cubre el plano entre las coordenadas (-2, -2) -> (2, 2), con
	// 51 muestras en cada dimensión
	vdc::UniformGrid<glm::vec2, float> uniformScalar{ glm::vec2{-2.0f, -2.0f}, glm::vec2{2.0f, 2.0f}, {51, 51} };
	fillSamples(uniformScalar);

	// Encontrar el valor mínimo y máximo de las muestras
	auto uniformMinMax = findMinMaxSamples(uniformScalar);

	// Encontrar la celda con la máxima variación entre sus muestras
	auto uniformMaxRange = findMaxRangeCell(uniformScalar);

	// Mostrar resultados
	reportResults(uniformScalar, uniformMinMax, uniformMaxRange);

	// unstructScalar es una malla no estructurada con 51*51 muestras y 50*50*2 celdas triangulares
	vdc::UnstructuredTriangleGrid<glm::vec2, float> unstructScalar{ 51 * 51, 50 * 50 * 2 };
	buildTriangularGrid(unstructScalar);

	// Encontrar el valor mínimo y máximo de las muestras
	auto unstructuredMinMax = findMinMaxSamples(unstructScalar);

	// Encontrar la celda con la máxima variación entre sus muestras
	auto unstructuredMaxRangeCell = findMaxRangeCell(unstructScalar);

	// Mostrar resultados
	reportResults(unstructScalar, unstructuredMinMax, unstructuredMaxRangeCell);

	return 0;
}
