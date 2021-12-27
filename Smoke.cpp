#include "Smoke.h"

const int SMOKE_SIZE = 128;

Smoke::Smoke()
{
	for (int i = 0; i < SMOKE_SIZE; i++)
	{
		Sphere* sphere = new Sphere();
		sphere->updatePosition(glm::dvec3(0.0));
		puffClouds.push_back(sphere);
		ObjectRenderer* renderer = new ObjectRenderer("moon_shader");
		renderer->init(sphere->getVertices(), sphere->getIndices());
		renderers.push_back(renderer);
	}
}

void Smoke::puff(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos, glm::dvec3 position)
{
	int _position = rand() % 16 + 8;
	double dPosition = _position / 10000.0;
	position = position + dPosition;

	for (int puffIndex = 0; puffIndex < SMOKE_SIZE; puffIndex++)
	{
		Sphere* cloud = puffClouds[puffIndex];
		float cSize = cloud->getSize();
		if (cSize > 0.0f)
		{
			cSize -= 0.00078125f;
			position = cloud->getPosition();
		}
		else
		{
			int _size = rand() % 18 + 12;
			cSize = _size / 10000.0f;
			cloud->updatePosition(position);
		}

		cloud->updateSize(cSize);
		
		ObjectRenderer* renderer = renderers[puffIndex];
		Shader* shader = renderer->getShader();
		shader->use();
		shader->setVec3("objectColor", 0.55f, 0.55f, 0.55f);
		shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader->setVec3("lightPos", _lightPos);
		renderer->render(projection, view, cSize, position);
	}
}

Smoke::~Smoke()
{
	for (int i = 0; i < SMOKE_SIZE; i++)
	{
		delete puffClouds[i];
		delete renderers[i];
	}
}