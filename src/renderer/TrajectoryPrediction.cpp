#include "TrajectoryPrediction.h"

using namespace ofsim_simulation;

void TrajectoryPrediction::initWithPositions(
	std::vector<double> px,
	std::vector<double> py,
	std::vector<double> pz,
	std::vector<TelemetryData> telemetryHistory)
{
	spheresHistory.clear();
	spheresPrediction.clear();

	for (int i = 0; i < px.size(); i++)
	{
		auto sphere = std::make_unique<ofsim_math_and_physics::Sphere>(3, 32, 16);
        sphere->position = dvec3(px[i], py[i], pz[i]);
		spheresPrediction.push_back(std::move(sphere));
	}

	for (int i = 0; i < telemetryHistory.size(); i += 8)
	{
		auto sphere = std::make_unique<ofsim_math_and_physics::Sphere>(3, 32, 16);
        sphere->position = telemetryHistory[i].position;
		spheresHistory.push_back(std::move(sphere));
	}

	if (spheresPrediction.size() > 0)
	{
		ofsim_math_and_physics::Sphere* sphere = spheresPrediction[0].get();
        renderer->init(sphere->verticesAndNormals, sphere->indices);
	}
}

void TrajectoryPrediction::render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos)
{
	renderHistory(projection, view, lightPos);
	renderPrediction(projection, view, lightPos);
}

void TrajectoryPrediction::renderHistory(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos)
{
	Shader* shader = renderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.15f, 0.95f, 0.15f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", lightPos);

	for (int i = 0; i < spheresHistory.size(); i++)
	{
        renderer->render(projection, view, spheresHistory[i]->size, spheresHistory[i]->position);
	}
}

void TrajectoryPrediction::renderPrediction(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos)
{
	Shader* shader = renderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.95f, 0.15f, 0.15f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", lightPos);

	for (int i = 2; i < spheresPrediction.size(); i++)
	{
        renderer->render(projection, view, spheresPrediction[i]->size, spheresPrediction[i]->position);
	}
}
