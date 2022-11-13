#include "..\TrajectoryPrediction.h"

TrajectoryPrediction::TrajectoryPrediction()
{
	renderer = new ObjectRenderer("moon_shader");
}

void TrajectoryPrediction::initWithPositions(
	std::vector<double> px,
	std::vector<double> py,
	std::vector<double> pz)
{
	for (int i = 0; i < spheres.size(); i++)
	{
		delete spheres[i];
	}

	spheres.clear();

	for (int i = 0; i < px.size(); i++)
	{
		Sphere* sphere = new Sphere(1000, 32, 16);
		sphere->updatePosition(glm::dvec3(px[i], py[i], pz[i]));
		spheres.push_back(sphere);
	}
}

void TrajectoryPrediction::render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos)
{
	Shader* shader = renderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.95f, 0.15f, 0.15f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", lightPos);

	for (int i = 0; i < spheres.size(); i++)
	{
		renderer->render(projection, view, spheres[i]->getSize(), spheres[i]->getPosition());
	}
}

TrajectoryPrediction::~TrajectoryPrediction()
{
	for (int i = 0; i < spheres.size(); i++)
	{
		delete spheres[i];
	}
}
