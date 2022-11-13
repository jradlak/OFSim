#pragma once

#include <vector>
#include "../world/Sphere.h"
#include "ObjectRenderer.h"

#include "Renderable.h"

class TrajectoryPrediction : public Renderable
{
public:
	TrajectoryPrediction();

	void initWithPositions(
		std::vector<double> px,
		std::vector<double> py,
		std::vector<double> pz);

	void render(glm::dmat4 projections, glm::dmat4 view, const glm::dvec3 lightPos);

	~TrajectoryPrediction();

private:
	std::vector<Sphere*> spheres;
	ObjectRenderer* renderer;
};