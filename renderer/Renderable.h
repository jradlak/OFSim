#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Renderable {
public:
    virtual void init() = 0;
    virtual void render(glm::dmat4 projections, glm::dmat4 view, glm::dvec3 lightPos) = 0;
};