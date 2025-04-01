
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
        if (std::abs(v2 - v1) < 1e-6) // Prevent division by zero
            return (p1 + p2) * 0.5f;
        
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

            // Calculate case index for marching squares (4-bit code)
            int caseIndex = 0;
            if (v00 > contourValue) caseIndex |= 1;
            if (v10 > contourValue) caseIndex |= 2;
            if (v11 > contourValue) caseIndex |= 4;
            if (v01 > contourValue) caseIndex |= 8;

            // Calculate intersection points
            glm::vec2 bottom, right, top, left;
            
            // Bottom edge (between v00 and v10)
            bottom = interpolate(v00, v10, p00, p10, contourValue);
            
            // Right edge (between v10 and v11)
            right = interpolate(v10, v11, p10, p11, contourValue);
            
            // Top edge (between v01 and v11)
            top = interpolate(v01, v11, p01, p11, contourValue);
            
            // Left edge (between v00 and v01)
            left = interpolate(v00, v01, p00, p01, contourValue);

            // Apply marching squares cases to add the correct line segments
            switch (caseIndex) {
                case 0: // No contour
                case 15: // Full contour (no lines)
                    break;
                    
                case 1: // Bottom-left corner
                case 14: // Inverse of case 1
                    positions.push_back(glm::vec3(bottom, 0.0f));
                    positions.push_back(glm::vec3(left, 0.0f));
                    break;
                    
                case 2: // Bottom-right corner
                case 13: // Inverse of case 2
                    positions.push_back(glm::vec3(bottom, 0.0f));
                    positions.push_back(glm::vec3(right, 0.0f));
                    break;
                    
                case 3: // Bottom side
                case 12: // Inverse of case 3
                    positions.push_back(glm::vec3(left, 0.0f));
                    positions.push_back(glm::vec3(right, 0.0f));
                    break;
                    
                case 4: // Top-right corner
                case 11: // Inverse of case 4
                    positions.push_back(glm::vec3(right, 0.0f));
                    positions.push_back(glm::vec3(top, 0.0f));
                    break;
                    
                case 5: // Bottom-left and top-right corners (ambiguous case)
                case 10: // Inverse of case 5
                {
                    // For ambiguous cases, we need to decide how to connect the points
                    // One approach is to check the center value
                    float centerValue = (v00 + v10 + v11 + v01) / 4.0f;
                    
                    if ((centerValue > contourValue && caseIndex == 5) || 
                        (centerValue <= contourValue && caseIndex == 10)) {
                        // Connect bottom to left and right to top
                        positions.push_back(glm::vec3(bottom, 0.0f));
                        positions.push_back(glm::vec3(left, 0.0f));
                        positions.push_back(glm::vec3(right, 0.0f));
                        positions.push_back(glm::vec3(top, 0.0f));
                    } else {
                        // Connect bottom to right and left to top
                        positions.push_back(glm::vec3(bottom, 0.0f));
                        positions.push_back(glm::vec3(right, 0.0f));
                        positions.push_back(glm::vec3(left, 0.0f));
                        positions.push_back(glm::vec3(top, 0.0f));
                    }
                    break;
                }
                    
                case 6: // Right side
                case 9: // Inverse of case 6
                    positions.push_back(glm::vec3(bottom, 0.0f));
                    positions.push_back(glm::vec3(top, 0.0f));
                    break;
                    
                case 7: // All except top-left
                case 8: // Top-left corner
                    positions.push_back(glm::vec3(left, 0.0f));
                    positions.push_back(glm::vec3(top, 0.0f));
                    break;
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
