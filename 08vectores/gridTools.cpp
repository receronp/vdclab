#include "gridTools.h"
#include <glm/glm.hpp>
#include <PGUPV.h>

using namespace PGUPV;

void vdc::sampleVectorField(vdc::Grid<glm::vec2, glm::vec2> &g, std::function<glm::vec2(const glm::vec2 &)> f) {
  for (size_t i = 0; i < g.numSamples(); i++) {
    g.setSampleValue(i, f(g.getSamplePosition(i)));
  }
}

/**

Completa esta función:

La función devuelve un modelo PGUPV con un segmento de línea (dos vértices) por cada muestra de la malla g. 
Dicha línea apunta en la dirección indicada por el valor de la muestra, y tiene una longitud de k.

*/

std::shared_ptr<Model> vdc::computeHedgeHog(const vdc::Grid<glm::vec2, glm::vec2> &g, float k) {
  auto result = std::make_shared<Model>();

  return result;
}

/**

Completa esta función:

La función devuelve una malla uniforme del mismo tamaño que la malla de entrada. En vez de ser 
una malla vectorial, será una malla escalar, cuyas muestras serán el valor de la divergencia de 
la muestra correspondiente de la malla de entrada:

*/


std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> vdc::computeDivergence(const vdc::UniformGrid<glm::vec2, glm::vec2> &g) {
  std::vector<int> dims{ g.getNumSamplesPerDimension(0), g.getNumSamplesPerDimension(1) };
  auto result = std::make_shared<vdc::UniformGrid<glm::vec2, float>>(g.getMinCoord(), g.getMaxCoord(), dims);

  // Borra el siguiente código
  for (size_t i = 0; i < result->numSamples(); i++) {
    result->setSampleValue(i, 0.0f);
  }

  return result;
}


/*
Completa esta función:

La función devuelve una malla escalar de la misma dimensión que la malla de entrada, donde cada muestra 
contiene la magnitud de la vorticidad de la muestra correspondiente de la malla de entrada.

*/

std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> vdc::computeVorticity(const vdc::UniformGrid<glm::vec2, glm::vec2> &g) {
  std::vector<int> dims{ g.getNumSamplesPerDimension(0), g.getNumSamplesPerDimension(1) };
  auto result = std::make_shared<vdc::UniformGrid<glm::vec2, float>>(g.getMinCoord(), g.getMaxCoord(), dims);

  // Borra el siguiente código
  for (size_t i = 0; i < result->numSamples(); i++) {
	  result->setSampleValue(i, 0.0f);
  }

  return result;
}

/*

Completa la siguiente función. 

La función devuelve una malla de PGUPV con los vértices, colores y draw command 
necesario para dibujar la línea de corriente que empieza en p0. El paso de integración 
se pasa en el parámetro dt, maxT es el tiempo máximo de integración y maxL es 
la longitud máxima de la línea de corriente.Por último, la línea se dibujará 
del color indicado por el último parámetro.
*/

std::shared_ptr<PGUPV::Mesh> vdc::computeStreamline(const vdc::UniformGrid<glm::vec2, glm::vec2> &g, glm::vec2 &p0, float dt, float maxT, float maxL, glm::vec4 color) {
	auto result = std::make_shared<PGUPV::Mesh>();

	return result;
}