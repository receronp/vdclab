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
  auto mesh = std::make_shared<PGUPV::Mesh>();

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec4> colors;

  float maxMagnitude = 0.0f;
  for (size_t i = 0; i < g.numSamples(); i++) {
    glm::vec2 vector = g.getSampleValue(i);
    float magnitude = glm::length(vector);
    if (magnitude > maxMagnitude) {
      maxMagnitude = magnitude;
    }
  }

  for (size_t i = 0; i < g.numSamples(); i++) {
    glm::vec2 position = g.getSamplePosition(i);
    glm::vec2 vector = g.getSampleValue(i);

    float magnitude = glm::length(vector);
    glm::vec2 direction = (magnitude > 0.0f) ? glm::normalize(vector) : glm::vec2(0.0f);

    glm::vec3 start(position, 0.0f);
    glm::vec3 end(position + k * direction, 0.0f);

    vertices.push_back(start);
    vertices.push_back(end);

    float normalizedMagnitude = maxMagnitude > 0.0f ? magnitude / maxMagnitude : 0.0f;
    glm::vec4 color = glm::vec4(normalizedMagnitude, 0.0f, 1.0f - normalizedMagnitude, 1.0f);

    colors.push_back(color);
    colors.push_back(color);
  }

  mesh->addVertices(vertices);
  mesh->addColors(colors);
  mesh->addDrawCommand(new PGUPV::DrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size())));
  result->addMesh(mesh);

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

  float dx = (g.getMaxCoord().x - g.getMinCoord().x) / (dims[0] - 1);
  float dy = (g.getMaxCoord().y - g.getMinCoord().y) / (dims[1] - 1);

  for (int j = 0; j < dims[1]; j++) {
    for (int i = 0; i < dims[0]; i++) {
      size_t idx = i + j * dims[0];

      float divergence = 0.0f;

      if (i > 0 && i < dims[0] - 1) {
        glm::vec2 left = g.getSampleValue(idx - 1);
        glm::vec2 right = g.getSampleValue(idx + 1);
        divergence += (right.x - left.x) / (2.0f * dx);
      }
      else if (i == 0) {
        glm::vec2 right = g.getSampleValue(idx + 1);
        glm::vec2 center = g.getSampleValue(idx);
        divergence += (right.x - center.x) / dx;
      }
      else if (i == dims[0] - 1) {
        glm::vec2 left = g.getSampleValue(idx - 1);
        glm::vec2 center = g.getSampleValue(idx);
        divergence += (center.x - left.x) / dx;
      }

      if (j > 0 && j < dims[1] - 1) {
        glm::vec2 bottom = g.getSampleValue(idx - dims[0]);
        glm::vec2 top = g.getSampleValue(idx + dims[0]);
        divergence += (top.y - bottom.y) / (2.0f * dy);
      }
      else if (j == 0) {
        glm::vec2 top = g.getSampleValue(idx + dims[0]);
        glm::vec2 center = g.getSampleValue(idx);
        divergence += (top.y - center.y) / dy;
      }
      else if (j == dims[1] - 1) {
        glm::vec2 bottom = g.getSampleValue(idx - dims[0]);
        glm::vec2 center = g.getSampleValue(idx);
        divergence += (center.y - bottom.y) / dy;
      }

      result->setSampleValue(idx, divergence);
    }
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

  float dx = (g.getMaxCoord().x - g.getMinCoord().x) / (dims[0] - 1);
  float dy = (g.getMaxCoord().y - g.getMinCoord().y) / (dims[1] - 1);

  for (int j = 0; j < dims[1]; j++) {
    for (int i = 0; i < dims[0]; i++) {
      size_t idx = i + j * dims[0];
      
      float vorticity = 0.0f;

      if (i > 0 && i < dims[0] - 1) {
        glm::vec2 left = g.getSampleValue(idx - 1);
        glm::vec2 right = g.getSampleValue(idx + 1);
        vorticity += (right.y - left.y) / (2.0f * dx);
      } else if (i == 0) {
        glm::vec2 right = g.getSampleValue(idx + 1);
        glm::vec2 center = g.getSampleValue(idx);
        vorticity += (right.y - center.y) / dx;
      } else if (i == dims[0] - 1) {
        glm::vec2 left = g.getSampleValue(idx - 1);
        glm::vec2 center = g.getSampleValue(idx);
        vorticity += (center.y - left.y) / dx;
      }

      if (j > 0 && j < dims[1] - 1) {
        glm::vec2 bottom = g.getSampleValue(idx - dims[0]);
        glm::vec2 top = g.getSampleValue(idx + dims[0]);
        vorticity -= (top.x - bottom.x) / (2.0f * dy);
      } else if (j == 0){
        glm::vec2 top = g.getSampleValue(idx + dims[0]);
        glm::vec2 center = g.getSampleValue(idx);
        vorticity -= (top.x - center.x) / dy;
      } else if (j == dims[1] - 1) {
        glm::vec2 bottom = g.getSampleValue(idx - dims[0]);
        glm::vec2 center = g.getSampleValue(idx);
        vorticity -= (center.x - bottom.x) / dy;
      }

      result->setSampleValue(idx, vorticity);
    }
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

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec4> colors;

  glm::vec2 currentPoint = p0;
  float totalTime = 0.0f;
  float totalLength = 0.0f;

  float maxDivergence = -std::numeric_limits<float>::infinity();
  float minDivergence = std::numeric_limits<float>::infinity();

  for (size_t i = 0; i < g.numSamples(); i++) {
    glm::vec2 position = g.getSamplePosition(i);

    float divergence = 0.0f;

    size_t idx = i;
    int dimsX = g.getNumSamplesPerDimension(0);
    int dimsY = g.getNumSamplesPerDimension(1);

    if (i % dimsX > 0 && i % dimsX < dimsX - 1) {
      glm::vec2 left = g.getSampleValue(idx - 1);
      glm::vec2 right = g.getSampleValue(idx + 1);
      divergence += (right.x - left.x) / (2.0f * (g.getMaxCoord().x - g.getMinCoord().x) / (dimsX - 1));
    }

    if (i / dimsX > 0 && i / dimsX < dimsY - 1) {
      glm::vec2 bottom = g.getSampleValue(idx - dimsX);
      glm::vec2 top = g.getSampleValue(idx + dimsX);
      divergence += (top.y - bottom.y) / (2.0f * (g.getMaxCoord().y - g.getMinCoord().y) / (dimsY - 1));
    }

    maxDivergence = std::max(maxDivergence, divergence);
    minDivergence = std::min(minDivergence, divergence);
  }

  vertices.push_back(glm::vec3(currentPoint, 0.0f));
  colors.push_back(color);

  while (totalTime < maxT && totalLength < maxL) {
    size_t cellIdx;
    if (!g.findCell(currentPoint, cellIdx)) {
      break;
    }

    glm::vec2 refCoords = g.world2cell(cellIdx, currentPoint);
    glm::vec2 vector = g.interpolateC1Square(cellIdx, refCoords);

    float divergence = 0.0f;

    int dimsX = g.getNumSamplesPerDimension(0);
    int dimsY = g.getNumSamplesPerDimension(1);

    if (cellIdx % dimsX > 0 && cellIdx % dimsX < dimsX - 1) {
      glm::vec2 left = g.getSampleValue(cellIdx - 1);
      glm::vec2 right = g.getSampleValue(cellIdx + 1);
      divergence += (right.x - left.x) / (2.0f * (g.getMaxCoord().x - g.getMinCoord().x) / (dimsX - 1));
    }

    if (cellIdx / dimsX > 0 && cellIdx / dimsX < dimsY - 1) {
      glm::vec2 bottom = g.getSampleValue(cellIdx - dimsX);
      glm::vec2 top = g.getSampleValue(cellIdx + dimsX);
      divergence += (top.y - bottom.y) / (2.0f * (g.getMaxCoord().y - g.getMinCoord().y) / (dimsY - 1));
    }

    float normalizedDivergence = (divergence - minDivergence) / (maxDivergence - minDivergence);

    glm::vec4 mapcolorDivergence;
    if (normalizedDivergence < 0.25f) {
      mapcolorDivergence = glm::mix(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), normalizedDivergence * 4.0f);
    } else if (normalizedDivergence < 0.5f) {
      mapcolorDivergence = glm::mix(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), (normalizedDivergence - 0.25f) * 4.0f);
    } else if (normalizedDivergence < 0.75f) {
      mapcolorDivergence = glm::mix(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), (normalizedDivergence - 0.5f) * 4.0f);
    } else {
      mapcolorDivergence = glm::mix(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), (normalizedDivergence - 0.75f) * 4.0f);
    }

    glm::vec2 nextPoint = currentPoint + vector * dt;

    float segmentLength = glm::length(nextPoint - currentPoint);
    totalLength += segmentLength;
    totalTime += dt;

    vertices.push_back(glm::vec3(nextPoint, 0.0f));
    colors.push_back(mapcolorDivergence);
    currentPoint = nextPoint;
  }

  result->addVertices(vertices);
  result->addColors(colors);

  result->addDrawCommand(new PGUPV::DrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(vertices.size())));

  return result;
}