
#include "contornos.h"
#include <mesh.h>
#include <drawCommand.h>
#include <uniformGrid.h>

using namespace PGUPV;

/**
Calcula el contorno en la malla para el valor indicado.
\param grid malla en la que se calcula el contorno
\param contourValue el valor por el que pasa la l�nea de contorno
\return un modelo PGUPV con los segmentos que componen la l�nea de contorno
*/
std::shared_ptr<Model> computeContourLine(const vdc::Grid<glm::vec2, float> &grid, float contourValue) {
    // Validate that the grid is of type UniformGrid
    auto uniformGrid = dynamic_cast<const vdc::UniformGrid<glm::vec2, float>*>(&grid);
    if (!uniformGrid) {
        // If the grid is not a UniformGrid, return an empty model
        return std::make_shared<PGUPV::Model>();
    }
    
    auto model = std::make_shared<PGUPV::Model>();
    auto mesh = std::make_shared<PGUPV::Mesh>();
    std::vector<glm::vec3> positions;

    // Get grid dimensions
    size_t width = uniformGrid->getNumSamplesPerDimension(0);
    size_t height = uniformGrid->getNumSamplesPerDimension(1);

    if (width < 2 || height < 2)
        return model;

    // Lambda function to interpolate between two points
    auto interpolate = [](float v1, float v2, const glm::vec2 &p1, const glm::vec2 &p2, float contourValue) {
        float t = (contourValue - v1) / (v2 - v1);
        return glm::mix(p1, p2, t);
    };

    // Iterate through each cell in the grid
    for (size_t j = 0; j < height - 1; j++) {
        for (size_t i = 0; i < width - 1; i++) {
            // Calculate the sample indices for the four corners of the cell
            size_t idx00 = i + j * width;
            size_t idx10 = (i + 1) + j * width;
            size_t idx01 = i + (j + 1) * width;
            size_t idx11 = (i + 1) + (j + 1) * width;

            // Get the values at the four corners of the cell
            float v00 = grid.getSampleValue(idx00);
            float v10 = grid.getSampleValue(idx10);
            float v01 = grid.getSampleValue(idx01);
            float v11 = grid.getSampleValue(idx11);

            // Get the positions of the four corners of the cell
            glm::vec2 p00 = grid.getSamplePosition(idx00);
            glm::vec2 p10 = grid.getSamplePosition(idx10);
            glm::vec2 p01 = grid.getSamplePosition(idx01);
            glm::vec2 p11 = grid.getSamplePosition(idx11);

            // Check for intersections with the contour value
            std::vector<glm::vec3> segmentPoints;

            if ((v00 <= contourValue && v10 > contourValue) || (v00 > contourValue && v10 <= contourValue)) {
                segmentPoints.push_back(glm::vec3(interpolate(v00, v10, p00, p10, contourValue), 0.0f));
            }
            if ((v10 <= contourValue && v11 > contourValue) || (v10 > contourValue && v11 <= contourValue)) {
                segmentPoints.push_back(glm::vec3(interpolate(v10, v11, p10, p11, contourValue), 0.0f));
            }
            if ((v11 <= contourValue && v01 > contourValue) || (v11 > contourValue && v01 <= contourValue)) {
                segmentPoints.push_back(glm::vec3(interpolate(v11, v01, p11, p01, contourValue), 0.0f));
            }
            if ((v01 <= contourValue && v00 > contourValue) || (v01 > contourValue && v00 <= contourValue)) {
                segmentPoints.push_back(glm::vec3(interpolate(v01, v00, p01, p00, contourValue), 0.0f));
            }

            // Add line segments to the positions vector
            if (segmentPoints.size() == 2) {
                positions.push_back(segmentPoints[0]);
                positions.push_back(segmentPoints[1]);
            }
        }
    }

    // Check if any contour segments were found
    if (!positions.empty()) {
        mesh->addVertices(positions);
        mesh->addDrawCommand(new PGUPV::DrawArrays(GL_LINES, 0, static_cast<GLsizei>(positions.size())));
        model->addMesh(mesh);
    }

    return model;
}
