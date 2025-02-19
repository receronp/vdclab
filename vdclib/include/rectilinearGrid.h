#pragma once

#include "uniformGrid.h"
#include <vector>

namespace vdc {
	/**
	\class RectilinarGrid
	Implementaci�n de una malla rectil�nea.

	Ejemplo de uso:

	RectilinearGrid<glm::vec2, float> rg({-2.f, -0.5f, 0.f, 0.5f, 2.f}, {0.f, 1.f, 2.f});

	rg ser� una malla rectil�nea con 5x3 muestras, y 4x2 celdas. La malla cubre el �rea entre
	los puntos (-2, 2) y (0, 2). F�jate que las muestras no est�n uniformemente repartidas en 
	el eje X.
	El valor de las muestras es de tipo float, y se puede establecer con la llamada Grid::setSampleValue.

	*/


	template <typename CoordType, typename SampleType>
	class RectilinearGrid : public UniformGrid<glm::vec1, SampleType>
	{
	public:
		RectilinearGrid(std::vector<float> samplePosX) :
			UniformGrid<CoordType, SampleType>(glm::vec1(samplePosX.front()), glm::vec1(samplePosX.back()), { samplePosX.size() })
		{
			assert(0); // Sólo mallas de 1, 2 y 3D
		};
	};


	template <typename SampleType>
	class RectilinearGrid<glm::vec1, SampleType> :public UniformGrid<glm::vec1, SampleType>
	{
	public:
	using TopoQuadGrid<glm::vec1, SampleType>::getMeshPointIndices; 
		RectilinearGrid(const std::vector<float> &samplePosX) :
			UniformGrid<glm::vec1, SampleType>(
				glm::vec1(static_cast<glm::vec1::value_type>(samplePosX.front())),
				glm::vec1(samplePosX.back()),
				{ static_cast<int>(samplePosX.size()) }),
			samplePointsX(samplePosX) {
		};

		glm::vec1 getSamplePosition(size_t i) const override {
			auto coord = getMeshPointIndices(i);
			return glm::vec1(samplePointsX[coord.x]);
		};

	private:
		std::vector<float> samplePointsX;
	};

	template <typename SampleType>
	class RectilinearGrid<glm::vec2, SampleType> :public UniformGrid<glm::vec2, SampleType>
	{
	public:
		using TopoQuadGrid<glm::vec2, SampleType>::getMeshPointIndices; 
		RectilinearGrid(const std::vector<float> &samplePosX, const std::vector<float> &samplePosY) :
			UniformGrid<glm::vec2, SampleType>(
				glm::vec2(samplePosX.front(), samplePosY.front()),
				glm::vec2(samplePosX.back(), samplePosY.front()),
				{ static_cast<int>(samplePosX.size()), static_cast<int>(samplePosY.size()) }),
			samplePointsX(samplePosX), samplePointsY(samplePosY) {
		};

		glm::vec2 getSamplePosition(size_t i) const override {
			auto coord = getMeshPointIndices(i);
			return glm::vec2(samplePointsX[coord.x], samplePointsY[coord.y]);
		};
	private:
		std::vector<float> samplePointsX, samplePointsY;
	};


	template <typename SampleType>
	class RectilinearGrid<glm::vec3, SampleType> :public UniformGrid<glm::vec3, SampleType>
	{
	public:
		using TopoQuadGrid<glm::vec3, SampleType>::getMeshPointIndices; 
		RectilinearGrid(const std::vector<float> &samplePosX, const std::vector<float> &samplePosY, const std::vector<float> &samplePosZ) :
			UniformGrid<glm::vec3, SampleType>(
				glm::vec3(samplePosX.front(), samplePosY.front(), samplePosZ.front()),
				glm::vec3(samplePosX.back(), samplePosY.front(), samplePosZ.back()),
				{ static_cast<int>(samplePosX.size()), static_cast<int>(samplePosY.size()), static_cast<int>(samplePosZ.size()) }),
			samplePointsX(samplePosX), samplePointsY(samplePosY), samplePointsZ(samplePosZ) {
		};

		glm::vec3 getSamplePosition(size_t i) const override {
			auto coord = getMeshPointIndices(i);
			return glm::vec3(samplePointsX[coord.x], samplePointsY[coord.y], samplePointsZ[coord.z]);
		};
	private:
		std::vector<float> samplePointsX, samplePointsY, samplePointsZ;
	};
};

