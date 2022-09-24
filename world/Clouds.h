#pragma once
#include <vector>

#include "CelestialBody.h"
#include "..\renderer\ModelRenderer.h"

class Clouds
{
public:
	Clouds(const int _numberOfClouds, CelestialBody& _earth, double _angle, double _dangle);

	void provideInitialRotation(glm::dvec3 _initialRotation) { initialRotation = _initialRotation; }

	~Clouds() {}

private:
	const int numberOfClouds;
	std::vector<glm::dvec3> cloudsPositions;
	std::vector<double> cloudsSizes;

	double angle;
	double dangle;
	glm::dvec3 initialRotation;

	ModelRenderer* modelRenderer;
	CelestialBody& earth;

	void calculatePositionsAndSizes();
};