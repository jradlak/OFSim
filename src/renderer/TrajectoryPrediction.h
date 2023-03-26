#pragma once

#include <vector>
#include "../world/Sphere.h"
#include "../engine/TelemetryData.h"
#include "ObjectRenderer.h"


#include "Renderable.h"

class TrajectoryPrediction : public Renderable
{
public:
	TrajectoryPrediction();

	void initWithPositions(
		std::vector<double> px,
		std::vector<double> py,
		std::vector<double> pz,
		std::vector<TelemetryData> telemetryHistory);

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	
	~TrajectoryPrediction();

private:
	std::vector<Sphere*> spheresHistory;
	std::vector<Sphere*> spheresPrediction;
	ObjectRenderer* renderer;

	void clearSpheres();

	void renderHistory(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	void renderPrediction(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
};