#include "TriangleGeometry.h"

TriangleGeometry::TriangleGeometry()
{

}

std::vector<float> TriangleGeometry::getVertAN()
{
	return vertAN;
}

std::vector<int> TriangleGeometry::getIndices()
{
	return indices;
}

void TriangleGeometry::addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal)
{
	// TODO: fix normal calculations	
	if (normal == glm::vec3(0.0f)) 
	{		
		//normal = glm::cross(p3 - p1, p2 - p1);				
	}

	addVert(p1, normal);
	addVert(p2, normal);
	addVert(p3, normal);
}

void TriangleGeometry::addVert(glm::vec3 vert, glm::vec3 normal)
{
	int index = indexOfVert(vert);

	if (index == -1)
	{
		int pos = vertAN.size();

		vertAN.insert(vertAN.end(), {
				vert.x, vert.y, vert.z,
				normal.x, normal.y, normal.z
			});

		index = pos;
	}

	indices.push_back(index / 6);
}

void TriangleGeometry::normalize()
{
	double h = 0.016 / 2.0;
	for (int i = 2; i < vertAN.size(); i += 6)
	{
		vertAN[i] = vertAN[i] - h;
	}
}

int TriangleGeometry::indexOfVert(glm::vec3 vert)
{
	if (vertAN.size() < 6) {
		return -1;
	}

	for (unsigned int i = 0; i <= vertAN.size() - 6; i += 6)
	{
		if (
			vert.x == vertAN[i] &&
			vert.y == vertAN[i + 1] &&
			vert.z == vertAN[i + 2]
			)
		{
			return i;
		}
	}

	return -1;
}

TriangleGeometry::~TriangleGeometry()
{

}
