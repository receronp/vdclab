#pragma once

#include <vector>
#include <type_traits>

#include "grid.h"

namespace vdc {
	/**

	\class TopoQuadGrid
	Clase abstracta que implementa la topología de una malla sin agujeros de celdas con 
	cuatro vértices (de ella derivan la malla uniforme, rectilínea y estructurada).

	*/

	template <typename CoordType, typename SampleType>
	class TopoQuadGrid : public Grid<CoordType, SampleType> {
	public:
		static constexpr typename CoordType::length_type NDIM = sizeof(CoordType) / sizeof(typename CoordType::value_type);

		/**
		Constructor. La dimensión de la malla puede estar entre 1 y 4. La dimensión la especifica
		el tipo de las coordenadas (CoordType). Por ejemplo, si CoordType es glm::vec3, la malla
		será tridimensional.
		\param numSamplesPerDim std::vector con el número de muestras por cada dimensión.

		*/
		TopoQuadGrid(const std::vector<int> &numSamplesPerDim) :
			nSamplesPerDim(numSamplesPerDim) {

			static_assert(NDIM > 0 && NDIM <= 4, "Sólo se soportan mallas de 1 a 4 dimensiones");
			totalSamples = nSamplesPerDim[0];
			totalCells = nSamplesPerDim[0] - 1;
			sampleStrides[0] = 1;
			for (glm::uvec4::length_type i = 1; i < NDIM; i++) {
				sampleStrides[i] = static_cast<glm::uvec4::value_type>(totalSamples);
				totalSamples *= nSamplesPerDim[i];
				cellStrides[i] = static_cast<glm::uvec4::value_type>(totalCells);
				totalCells *= nSamplesPerDim[i] - 1;
			}
		};

		virtual ~TopoQuadGrid() {};

		size_t numSamples() const override {
			return totalSamples;
		}

		size_t numCells() const  override {
			return totalCells;
		}

		size_t getCellSamples(size_t c, size_t *v) const override {
			glm::uvec4 meshIndices;
			for (int i = static_cast<int>(NDIM - 1); i > 0; i--) {
				meshIndices[i] = static_cast<glm::uint>(c / cellStrides[i]);
				c = (c % cellStrides[i]);
			}
			meshIndices[0] = static_cast<glm::uint>(c);

			auto p = meshIndices * sampleStrides;

			size_t offset = p[0];
			for (int i = 1; i < NDIM; i++) {
				offset += p[i];
			}
			v[0] = offset;
			v[1] = v[0] + 1;
			v[2] = v[1] + nSamplesPerDim[0];
			v[3] = v[0] + nSamplesPerDim[0];

			return 4;
		}

		int getNumSamplesPerDimension(size_t dim) const { return nSamplesPerDim[dim]; }
		int getNumCellsPerDimension(size_t dim) const { return nSamplesPerDim[dim] - 1; }

	protected:
		std::vector<int> nSamplesPerDim;	// Número de muestras por dimensión

		glm::uvec4 getMeshPointIndices(size_t c) const {
			glm::uvec4 res;
			for (int i = static_cast<int>(NDIM - 1); i > 0; i--) {
				res[i] = static_cast<glm::uvec4::value_type>(c / sampleStrides[i]);
				c = (c % sampleStrides[i]);
			}
			res[0] = static_cast<glm::uvec4::value_type>(c);
			return res;
		}

		size_t totalSamples, totalCells;	// Cantidad de muestras y celdas
		glm::uvec4 sampleStrides;			// Saltos de una muestra a la siguiente por dimensión
		glm::uvec4 cellStrides;				// Saltos de una celda a la siguiente por dimensión
	};
};
