#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class TriangleGeometry
{

public:
	TriangleGeometry();

	std::vector<float> getVertAN();
	std::vector<int> getIndices();

	void addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal = glm::vec3(0.0f));
	int indexOfVert(glm::vec3 vert);
	void addVert(glm::vec3 vert, glm::vec3 normal);

	~TriangleGeometry();

private:
	std::vector<float> vertAN; // vertices and normals
	std::vector<int> indices;
};

