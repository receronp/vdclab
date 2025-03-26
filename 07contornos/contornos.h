#pragma once

#include <model.h>
#include <grid.h>

std::shared_ptr<PGUPV::Model> computeContourLine(const vdc::Grid<glm::vec2, float> &grid, float contourValue);
