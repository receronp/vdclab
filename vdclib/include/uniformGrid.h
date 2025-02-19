#pragma once

#include <vector>
#include <type_traits>

#include "topoQuadGrid.h"

namespace vdc {
	/**

	\class UniformGrid
	Implementación de una malla uniforme.

	Ejemplo de uso:

	UniformGrid<glm::vec2, float> uni(glm::vec2(0.0f, 0.0f), glm::vec2(10.f, 10.f), {11, 11});

	uni será una malla bidimensional que cubre el espacio entre (0,0) y (10, 10), y tiene
	11x11 muestras (10x10 celdas). Las muestras están uniformemente distribuidas en dicho espacio.
	El valor de las muestras es de tipo float, y se puede establecer con la llamada Grid::setSampleValue.

	*/

	template <typename CoordType, typename SampleType>
	class UniformGrid : public TopoQuadGrid<CoordType, SampleType> {
	public:
		using TopoQuadGrid<CoordType, SampleType>::nSamplesPerDim;
		using TopoQuadGrid<CoordType, SampleType>::cellStrides;
		using TopoQuadGrid<CoordType, SampleType>::getCellSamples;
		using TopoQuadGrid<CoordType, SampleType>::getMeshPointIndices;
		
	
		static constexpr typename CoordType::length_type NDIM = sizeof(CoordType) / sizeof(typename CoordType::value_type);

		/**
		Constructor. La dimensión de la malla puede estar entre 1 y 4. La dimensión la especifica
		el tipo de las coordenadas (CoordType). Por ejemplo, si CoordType es glm::vec3, la malla
		será tridimensional.
		\param min Coordenada mínima de la malla
		\param max Coordenada máxima de la malla
		\param numSamplesPerDim std::vector con el número de muestras por cada dimensión.

		*/
		UniformGrid(CoordType min, CoordType max, const std::vector<int> &numSamplesPerDim) :
			TopoQuadGrid<CoordType, SampleType>(numSamplesPerDim), minCoord(min), maxCoord(max) {
			CoordType div;
			for (typename CoordType::length_type i = 0; i < NDIM; i++)
				div[i] = static_cast<typename CoordType::value_type>(numSamplesPerDim[i] - 1);
			deltas = (max - min) / div;
		};

		virtual ~UniformGrid() {};

		bool findCell(const CoordType &p, size_t &cellIdx) const override
		{
			int C[NDIM];

			CoordType c = (p - minCoord) / deltas;
			for (typename CoordType::length_type i = 0; i < NDIM; i++) {
				C[i] = static_cast<int>(std::floor(c[i]));
				if (C[i] < 0 || C[i] >= nSamplesPerDim[i] - 1) return false;
			}
			cellIdx = C[0];
			for (int i = 1; i < NDIM; i++) {
				cellIdx += C[i] * cellStrides[i];
			}
			return true;
		}

		CoordType getSamplePosition(size_t i) const override
		{
			auto pointIndices = static_cast<CoordType>(getMeshPointIndices(i));
			return minCoord + deltas * pointIndices;
		}

		CoordType getMinCoord() const { return minCoord; }
		CoordType getMaxCoord() const { return maxCoord; }

		CoordType world2cell(size_t c, const CoordType & world) const override {
			size_t v[4];
			getCellSamples(c, v);
			auto rc = (world - getSamplePosition(v[0]))/ deltas;
			return rc;
		}
	protected:
		CoordType minCoord, maxCoord; // Coordenadas mínimas en esta malla
	private:
		CoordType deltas; // Tamaños de celda
	};
};
