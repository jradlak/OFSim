#pragma once
#include <vector>

#include "CelestialBody.h"
#include "..\src\renderer\ModelRenderer.h"
#include "..\src\renderer\Renderable.h"

class CloudsAndTrees : public Renderable
{
public:
	CloudsAndTrees(const int _numberOfClouds, CelestialBody& _earth,
		double _angle, double _dangle);

	void provideInitialRotation(glm::dvec3 _initialRotation) { initialRotation = _initialRotation; }

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);

	~CloudsAndTrees() {}

private:
	const int numberOfClouds;
	std::vector<glm::dvec3> cloudsPositions;
	std::vector<glm::dvec3> treesPositions;

	std::vector<double> cloudsSizes;

	double angle;
	double dangle;
	glm::dvec3 initialRotation;

	ModelRenderer* cloudRenderer;
	ModelRenderer* treeRenderer;
	
	CelestialBody& earth;

	void calculatePositionsAndSizes();
};