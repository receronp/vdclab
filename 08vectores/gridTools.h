#pragma once

#include <memory>
#include <functional>
#include <uniformGrid.h>
#include <glm/fwd.hpp>


namespace PGUPV {
  class Model;
  class Mesh;
}

namespace vdc {

  void sampleVectorField(vdc::Grid<glm::vec2, glm::vec2> &g, std::function<glm::vec2(const glm::vec2 &)> f);
  std::shared_ptr<PGUPV::Model> computeHedgeHog(const vdc::Grid<glm::vec2, glm::vec2> &g, float k);
  std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> computeDivergence(const vdc::UniformGrid<glm::vec2, glm::vec2> &g);
  std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> computeVorticity(const vdc::UniformGrid<glm::vec2, glm::vec2> &g);
  std::shared_ptr<PGUPV::Mesh> computeStreamline(const vdc::UniformGrid<glm::vec2, glm::vec2> &g, glm::vec2 &p0, float dt, float maxT, float maxL, glm::vec4 color);
};