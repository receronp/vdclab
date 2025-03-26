
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <algorithm>

#include "colormap.h"

/*

Completa las funciones de este fichero.
Cada función devuelve un vector con los colores que componen cada tipo de mapa de color.

*/

/**
\return un mapa de color aleatorio (256 colores elegidos aleatoriamente)
*/
std::vector<glm::vec4> createRandomColorMap() {
  std::vector<glm::vec4> colormap;
  for (unsigned int i = 0; i < 256; i++) {
    colormap.push_back(glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0f)));
  }
  return colormap;
}


/**
\return los colores de un mapa de niveles de gris
*/
std::vector<glm::vec4> createGrayscaleColorMap() {
  std::vector<glm::vec4> colormap;
  return colormap;
}

/**
\return los colores de un mapa arco iris
*/
std::vector<glm::vec4> createRainbowColorMap() {
  std::vector<glm::vec4> colormap;
  return colormap;
}

/**
\return los colores de un mapa de dos matices
\param low el color asociado a los valores bajos
\param high el color asociado a los valores altos
*/

std::vector<glm::vec4> createTwoColorMap(const glm::vec4 &low, const glm::vec4 &high) {
  std::vector<glm::vec4> colormap;
  return colormap;
}

/**
\return los colores de un mapa de calor
*/
std::vector<glm::vec4> createHeatColorMap() {
  std::vector<glm::vec4> colormap;
  return colormap;
}

/**
\return los colores de un mapa divergente
\param low el color asociado al valor más bajo
\param mid el color asociado al valor medio
\param high el color asociado al valor más alto
*/
std::vector<glm::vec4> createDivergingColorMap(const glm::vec4 &low, const glm::vec4 &mid, const glm::vec4 &high) {
  std::vector<glm::vec4> colormap;
  return colormap;
}


/**
\return los colores de un mapa de color de cebra
\param one un color
\param two otro color (para funcionar ambos colores deben tener una luminancia muy distinta)
*/

std::vector<glm::vec4> createZebraColorMap(const glm::vec4 &one, const glm::vec4 &two) {
  std::vector<glm::vec4> colormap;
  return colormap;
}
