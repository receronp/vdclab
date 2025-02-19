#pragma once

#include <vector>

#include "topoQuadGrid.h"

namespace vdc {

	/**
	\class StructuredGrid
	Implementación de una malla estructurada.

	Ejemplo de uso:

	StructuredGrid<glm::vec2, float> sg({100, 100});

	size_t i = 0;
	for (int y...
	  for (int x...
		 sg.setSamplePosition(i, glm::vec2(...));

	sg es una malla estructurada formada por 100x100 muestras, y 99x99 celdas.
	La posición de las muestras se define explícitamente mediante el método StructuredGrid::setSamplePosition.
	La topología es igual que la de una malla uniforme.
	El valor de las muestras es de tipo float, y se puede establecer con la llamada Grid::setSampleValue.

	*/
	template <typename CoordType, typename SampleType>
	class StructuredGrid : public TopoQuadGrid<CoordType, SampleType>
	{
	public:
		/**
		Constructor. La dimensión de la malla puede estar entre 1 y 4, y está definida por el número de
		elementos del vector numSamplesPerDim.
		\param numSamplesPerDim std::vector con el número de muestras que hay en cada dimensión
		*/
		StructuredGrid(std::vector<int> numSamplesPerDim) : TopoQuadGrid<CoordType, SampleType>(numSamplesPerDim)
		{
			assert(numSamplesPerDim.size() > 0 && numSamplesPerDim.size() <= 4);
			size_t numSamples = numSamplesPerDim[0];
			for (size_t i = 1; i < numSamplesPerDim.size(); i++) {
				assert(numSamplesPerDim[i] > 1);
				numSamples *= numSamplesPerDim[i];
			}
			points.resize(numSamples);
		}

		CoordType getSamplePosition(size_t i) const override {
			return points[i];
		}

		/**
		Establece las coordenadas (posición) de una muestra
		\param i la muestra cuya posición se desea definir
		\param p la coordenada de la muestra
		*/
		void setSamplePosition(size_t i, CoordType p) {
			points[i] = p;
		}

		bool findCell(const CoordType &p, size_t &cellIdx) const override {
			return false;
		}

	private:
		std::vector<CoordType>	points;		// Coordenadas explícitas de todas las muestras
	};
};
