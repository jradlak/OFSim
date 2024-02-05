#pragma once
#include <vector>
#include <memory>

#include "CelestialBody.h"
#include "../renderer/ModelRenderer.h"

class CloudsAndTrees
{
public:
	CloudsAndTrees(const int _numberOfClouds, CelestialBody& _earth,
		double _angle, double _dangle);

	void provideInitialRotation(glm::dvec3 _initialRotation) { initialRotation = _initialRotation; }

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);

private:	
	std::unique_ptr<ofsim_renderer::ModelRenderer> cloudRenderer;
	std::unique_ptr<ofsim_renderer::ModelRenderer> treeRenderer;

	const int numberOfClouds;

	std::vector<glm::dvec3> cloudsPositions;
	std::vector<glm::dvec3> treesPositions;

	std::vector<double> cloudsSizes;

	double angle;
	double dangle;
	glm::dvec3 initialRotation;

	CelestialBody& earth;

	void calculatePositionsAndSizes();
};