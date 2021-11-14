#include "Rocket.h"

Rocket::Rocket(std::string shaderName, glm::dvec3& _position, double _size)
	: position(_position), size(_size)
{
	objectRenderer = new ObjectRenderer(shaderName);
}

void Rocket::init()
{
	makeRocketGeometry();
	objectRenderer->init(vertAN, indices);
}

void Rocket::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	//TODO: make it cleaner!!!
	Shader* shader = objectRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.55f, 0.55f, 0.55f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);
	objectRenderer->render(projection, view, _lightPos, position, size);
}

void Rocket::updatePosition(glm::dvec3 newPosition)
{
	position = newPosition;
}

void Rocket::makeRocketGeometry()
{
	addTriangle(
		point(0.0, 0.016001, 0.0),        //0
		point(-0.001, 0.014001, -0.001),  //1
		point(0.001, 0.014002, -0.001)    //2
	);

	addTriangle(
		point(0.0, 0.016002, 0.0),        //0
		point(0.001, 0.014003, -0.001),   //2
		point(0.001, 0.014004, 0.001)     //3
	);

	addTriangle(
		point(0.0, 0.016003, 0.0),		   //0
		point(0.001, 0.014005, 0.001),    //3
		point(-0.001, 0.014006, 0.001)    //4
	);

	addTriangle(
		point(0.0, 0.016004, 0.0),        //0
		point(-0.001, 0.014007, 0.001),   //4
		point(-0.001, 0.014008, -0.001)   //1
	);
}

void Rocket::addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 normal = glm::cross(p3 - p1, p2 - p1);
	addVert(p1, normal);
	addVert(p2, normal);
	addVert(p3, normal);
}

void Rocket::addVert(glm::vec3 vert, glm::vec3 normal)
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

int Rocket::indexOfVert(glm::vec3 vert)
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

glm::vec3 Rocket::point(double x, double y, double z)
{
	return glm::vec3(x, y, z);
}

Rocket::~Rocket()
{
	delete objectRenderer;
}