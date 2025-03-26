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

Completa esta funci�n:

La funci�n devuelve un modelo PGUPV con un segmento de l�nea (dos v�rtices) por cada muestra de la malla g. 
Dicha l�nea apunta en la direcci�n indicada por el valor de la muestra, y tiene una longitud de k.

*/

std::shared_ptr<Model> vdc::computeHedgeHog(const vdc::Grid<glm::vec2, glm::vec2> &g, float k) {
  auto result = std::make_shared<Model>();

  return result;
}

/**

Completa esta funci�n:

La funci�n devuelve una malla uniforme del mismo tama�o que la malla de entrada. En vez de ser 
una malla vectorial, ser� una malla escalar, cuyas muestras ser�n el valor de la divergencia de 
la muestra correspondiente de la malla de entrada:

*/


std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> vdc::computeDivergence(const vdc::UniformGrid<glm::vec2, glm::vec2> &g) {
  std::vector<int> dims{ g.getNumSamplesPerDimension(0), g.getNumSamplesPerDimension(1) };
  auto result = std::make_shared<vdc::UniformGrid<glm::vec2, float>>(g.getMinCoord(), g.getMaxCoord(), dims);

  // Borra el siguiente c�digo
  for (size_t i = 0; i < result->numSamples(); i++) {
    result->setSampleValue(i, 0.0f);
  }

  return result;
}


/*
Completa esta funci�n:

La funci�n devuelve una malla escalar de la misma dimensi�n que la malla de entrada, donde cada muestra 
contiene la magnitud de la vorticidad de la muestra correspondiente de la malla de entrada.

*/

std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> vdc::computeVorticity(const vdc::UniformGrid<glm::vec2, glm::vec2> &g) {
  std::vector<int> dims{ g.getNumSamplesPerDimension(0), g.getNumSamplesPerDimension(1) };
  auto result = std::make_shared<vdc::UniformGrid<glm::vec2, float>>(g.getMinCoord(), g.getMaxCoord(), dims);

  // Borra el siguiente c�digo
  for (size_t i = 0; i < result->numSamples(); i++) {
	  result->setSampleValue(i, 0.0f);
  }

  return result;
}

/*

Completa la siguiente funci�n. 

La funci�n devuelve una malla de PGUPV con los v�rtices, colores y draw command 
necesario para dibujar la l�nea de corriente que empieza en p0. El paso de integraci�n 
se pasa en el par�metro dt, maxT es el tiempo m�ximo de integraci�n y maxL es 
la longitud m�xima de la l�nea de corriente.Por �ltimo, la l�nea se dibujar� 
del color indicado por el �ltimo par�metro.
*/

std::shared_ptr<PGUPV::Mesh> vdc::computeStreamline(const vdc::UniformGrid<glm::vec2, glm::vec2> &g, glm::vec2 &p0, float dt, float maxT, float maxL, glm::vec4 color) {
	auto result = std::make_shared<PGUPV::Mesh>();

	return result;
}