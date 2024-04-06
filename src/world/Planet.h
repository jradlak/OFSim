#pragma once

#include <memory>

#include "CloudsAndTrees.h"
#include "CelestialBody.h"
#include "Launchpad.h"
#include "../math_and_physics/MathTypes.h"

class Planet
{
public:
	Planet(f64 _size, const glm::dvec3& _position);

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	
	void init(i32 number, f64 _angle, f64 _dangle, dvec3 _rotation);

	dvec3 pointAboveTheSurface(f64 theta, f64 phi, f64 distance);

private:
	std::unique_ptr<CelestialBody> celestialBody = nullptr;
	std::unique_ptr<CloudsAndTrees> cloudsAndTrees = nullptr;
	std::unique_ptr<Launchpad> launchpad = nullptr;
};