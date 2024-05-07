#include "Smoke.h"

const int SMOKE_SIZE = 128;

using namespace ofsim_world;
using namespace ofsim_math_and_physics;

Smoke::Smoke()
{
	for (int i = 0; i < SMOKE_SIZE; i++)
	{
		std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
        sphere->position = dvec3(0.0);
		puffClouds.push_back(std::move(sphere));
	}

	Sphere* sphere = puffClouds[0].get();
	renderer = std::make_unique<ObjectRenderer>("moon_shader");
    renderer->init(sphere->verticesAndNormals, sphere->indices);
}

void Smoke::puff(dmat4& projection, dmat4& view, dvec3& _lightPos, dvec3 position)
{
    i32 _position = rand() % 16 + 8;
	double dPosition = _position / 10000.0;
	position = position + dPosition;

	for (int puffIndex = 0; puffIndex < SMOKE_SIZE; puffIndex++)
	{
		Sphere* cloud = puffClouds[puffIndex].get();
        float cSize = cloud->size;
		if (cSize > 0.0f)
		{
			cSize -= 0.00078125f;
            position = cloud->position;
		}
		else
		{
			int _size = rand() % 18 + 12;
			cSize = _size / 10000.0f;
            cloud->position = position;
		}

        cloud->size = cSize;
				
		Shader* shader = renderer->getShader();
		shader->use();
		shader->setVec3("objectColor", 0.85f, 0.85f, 0.85f);
		shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader->setVec3("lightPos", _lightPos);
		renderer->render(projection, view, cSize, position);
	}
}
