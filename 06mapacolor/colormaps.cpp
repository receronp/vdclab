
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
  for (unsigned int i = 0; i < 256; i++) {
      float t = i / 255.0f;
      colormap.push_back(glm::vec4(t, t, t, 1.0f));
  }
  return colormap;
}

/**
\return los colores de un mapa arco iris
*/
std::vector<glm::vec4> createRainbowColorMap() {
  std::vector<glm::vec4> colormap;
  for (unsigned int i = 0; i < 256; i++) {
      float t = i / 255.0f;
      if (t < 0.2f) {
          // Blue to Cyan
          float s = t / 0.2f;
          colormap.push_back(glm::vec4(0.0f, s, 1.0f, 1.0f));
      } else if (t < 0.4f) {
          // Cyan to Green
          float s = (t - 0.2f) / 0.2f;
          colormap.push_back(glm::vec4(0.0f, 1.0f, 1.0f - s, 1.0f));
      } else if (t < 0.6f) {
          // Green to Yellow
          float s = (t - 0.4f) / 0.2f;
          colormap.push_back(glm::vec4(s, 1.0f, 0.0f, 1.0f));
      } else if (t < 0.8f) {
          // Yellow to Orange
          float s = (t - 0.6f) / 0.2f;
          colormap.push_back(glm::vec4(1.0f, 1.0f - s * 0.5f, 0.0f, 1.0f));
      } else {
          // Orange to Red
          float s = (t - 0.8f) / 0.2f;
          colormap.push_back(glm::vec4(1.0f, 0.5f - s * 0.5f, 0.0f, 1.0f));
      }
  }
  return colormap;
}

/**
\return los colores de un mapa de dos matices
\param low el color asociado a los valores bajos
\param high el color asociado a los valores altos
*/

std::vector<glm::vec4> createTwoColorMap(const glm::vec4 &low, const glm::vec4 &high) {
  std::vector<glm::vec4> colormap;
  colormap.reserve(256);
  
  for (unsigned int i = 0; i < 256; i++) {
    float t = static_cast<float>(i) / 255.0f;
      glm::vec4 color = (1.0f - t) * low + t * high;
      colormap.emplace_back(color);
      colormap.push_back(mix(low, high, t));
  }
  return colormap;
}

/**
\return los colores de un mapa de calor
*/
std::vector<glm::vec4> createHeatColorMap() {
  std::vector<glm::vec4> colormap;
  for (unsigned int i = 0; i < 256; i++) {
      float t = i / 255.0f;
      if (t < 0.25f) {
          // Black to dark red
          float s = t / 0.25f;
          colormap.push_back(glm::vec4(s * 0.7f, 0.0f, 0.0f, 1.0f));
      } else if (t < 0.5f) {
          // Dark red to bright red
          float s = (t - 0.25f) / 0.25f;
          colormap.push_back(glm::vec4(0.7f + s * 0.3f, 0.0f, 0.0f, 1.0f));
      } else if (t < 0.75f) {
          // Red to orange
          float s = (t - 0.5f) / 0.25f;
          colormap.push_back(glm::vec4(1.0f, s * 0.5f, 0.0f, 1.0f));
      } else {
          // Orange to bright red/white
          float s = (t - 0.75f) / 0.25f;
          colormap.push_back(glm::vec4(1.0f, 0.5f + s * 0.5f, s * 0.7f, 1.0f));
      }
  }
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
  for (unsigned int i = 0; i < 256; i++) {
      float t = i / 255.0f;
      if (t < 0.5f) {
          // Low to mid
          float s = t / 0.5f;
          colormap.push_back(mix(low, mid, s));
      } else {
          // Mid to high
          float s = (t - 0.5f) / 0.5f;
          colormap.push_back(mix(mid, high, s));
      }
  }
  return colormap;
}


/**
\return los colores de un mapa de color de cebra
\param one un color
\param two otro color (para funcionar ambos colores deben tener una luminancia muy distinta)
*/

std::vector<glm::vec4> createZebraColorMap(const glm::vec4 &one, const glm::vec4 &two) {  
  std::vector<glm::vec4> colormap;
  for (unsigned int i = 0; i < 256; i++) {
      int band = (i / 16) % 2;
      
      if (band == 0) {
          colormap.push_back(one);
      } else {
          colormap.push_back(two);
      }
  }
  return colormap;
}
