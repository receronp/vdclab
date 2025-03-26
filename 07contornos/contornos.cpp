
#include "contornos.h"
#include <mesh.h>
#include <drawCommand.h>

using namespace PGUPV;

/**
Calcula el contorno en la malla para el valor indicado.
\param grid malla en la que se calcula el contorno
\param contourValue el valor por el que pasa la línea de contorno
\return un modelo PGUPV con los segmentos que componen la línea de contorno
*/
std::shared_ptr<Model> computeContourLine(const vdc::Grid<glm::vec2, float> &grid, float contourValue) {
	auto model = std::make_shared<PGUPV::Model>();
	return model;
}
