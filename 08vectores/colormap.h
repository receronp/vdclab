
#include <vector>
#include <glm/fwd.hpp>


std::vector<glm::vec4> createRandomColorMap();
std::vector<glm::vec4> createGrayscaleColorMap();
std::vector<glm::vec4> createRainbowColorMap();
std::vector<glm::vec4> createTwoColorMap(const glm::vec4 &low, const glm::vec4 &high);
std::vector<glm::vec4> createHeatColorMap();
std::vector<glm::vec4> createDivergingColorMap(const glm::vec4 &low, const glm::vec4 &mid, const glm::vec4 &high);
std::vector<glm::vec4> createZebraColorMap(const glm::vec4 &one, const glm::vec4 &two);
