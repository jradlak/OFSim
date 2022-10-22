#pragma once

#include "..\renderer\Renderable.h"
#include "CelestialBody.h"

class Moon : public Renderable
{
public:
	Moon(double _size, glm::dvec3& _position);
	
	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos);

	~Moon();
private:
	CelestialBody* celestialBody;	
};