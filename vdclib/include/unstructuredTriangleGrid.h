#pragma once

#include "grid.h"
#include <vector>

namespace vdc {

	/**

	\class UnstructuredTriangleGrid
	Implementación de una malla no estructurada a base de triángulos. 
	
	Ejemplo de uso:
	UnstructuredTriangleGrid<glm::vec2, float> ug(100, 200);
	for (int i = 0; i < 100; i++) {
		ug.setSamplePosition(i, glm::vec2(...);
	}

	size_t vertexIndices[3];
	for (int i = 0; i < 200; i++) {
		vertexIndices[0] = ...;
		vertexIndices[1] = ...;
		vertexIndices[2] = ...;
		ug.setCell(i, vertexIndices);
	}

	ug será una malla no estructurada con 100 muestras y 200 celdas.
	La posición de las muestras se establece mediante el método setSamplePosition. Los índices que 
	componen una celda se establecen con el método setCell.

	El valor de las muestras es de tipo float, y se puede establecer con la llamada Grid::setSampleValue.
	*/

	template <typename CoordType, typename SampleType>
	class UnstructuredTriangleGrid : public Grid<CoordType, SampleType>
	{
	public:
		UnstructuredTriangleGrid(size_t numSamples, size_t numCells)
		{
			points.resize(numSamples);
			cells.resize(3 * numCells);
		}

		size_t	numSamples() const override
		{
			return points.size();
		}

		size_t	numCells() const override
		{
			return cells.size() / 3;
		}

		CoordType getSamplePosition(size_t sampleIdx) const override {
			return points[sampleIdx];
		}

		/**
		Establece las coordenadas (posición) de una muestra
		\param i la muestra cuya posición se desea definir
		\param p la coordenada de la muestra
		*/
		void setSamplePosition(size_t i, const CoordType &p) {
			points[i] = p;
		}

		size_t getCellSamples(size_t cellIdx, size_t *vtsIdx) const override {
			vtsIdx[0] = cells[3 * cellIdx];
			vtsIdx[1] = cells[3 * cellIdx + 1];
			vtsIdx[2] = cells[3 * cellIdx + 2];

			return 3;
		}

		void setCell(size_t cell, size_t * vertices)
		{
			cells[3 * cell] = vertices[0];
			cells[3 * cell + 1] = vertices[1];
			cells[3 * cell + 2] = vertices[2];
		}

		bool findCell(const CoordType &p, size_t &cellIDx) const override {
			return false;
		}

	protected:
		std::vector<CoordType> points;
		std::vector<size_t> cells;
	};
};