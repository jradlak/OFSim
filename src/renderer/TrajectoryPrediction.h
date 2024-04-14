#pragma once

#include <vector>
#include <memory>

#include "../math_and_physics/Sphere.h"
#include "../simulation/TelemetryData.h"
#include "ObjectRenderer.h"

class TrajectoryPrediction
{
public:
    TrajectoryPrediction() { renderer = std::make_unique<ObjectRenderer>("ball_shader"); }

	void initWithPositions(
		std::vector<double> px,
		std::vector<double> py,
		std::vector<double> pz,
		std::vector<TelemetryData> telemetryHistory);

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	
	~TrajectoryPrediction() {}

private:
	std::vector<std::unique_ptr<ofsim_math_and_physics::Sphere>> spheresHistory;
	std::vector<std::unique_ptr<ofsim_math_and_physics::Sphere>> spheresPrediction;
	
	std::unique_ptr<ObjectRenderer> renderer;

	void renderHistory(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	void renderPrediction(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
};
