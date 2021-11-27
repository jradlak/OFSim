#include "Rocket.h"

Rocket::Rocket(std::string shaderName, glm::dvec3& _position, double _size, glm::dvec3 _rotation)
	: position(_position), size(_size), rotation(_rotation)
{
	objectRenderer = new ObjectRenderer(shaderName);
	geometry = new TriangleGeometry();
	forces.push_back(glm::dvec3(0.0f, 9.81f, 0.0f));
	mass = 4.0;
}

void Rocket::init()
{
	makeRocketGeometry();
	objectRenderer->init(geometry->getVertAN(), geometry->getIndices());
}

void Rocket::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	//TODO: make it cleaner!!!
	Shader* shader = objectRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.55f, 0.55f, 0.55f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);
	objectRenderer->render(projection, view, size, position, rotation);
	//objectRenderer->render(projection, view, size, position, glm::dvec3(0.0));
}

glm::dvec3 Rocket::getPosition()
{
	return position;
}

glm::dvec3 Rocket::getVelocity()
{
	return velocity;
}

void Rocket::updatePhysics(double deltaTime)
{
	glm::vec3 sumOfForces = glm::vec3(0.0f);
	for (unsigned int i = 0; i < forces.size(); i++)
	{
		sumOfForces += forces[i];
	}

	glm::dvec3 force = sumOfForces;

	velocity += force / mass * deltaTime;
	position += velocity * deltaTime;
}

void Rocket::addForce(glm::vec3 force)
{
	forces.push_back(force);
}

void Rocket::resetForces()
{
	forces.clear();
	forces.push_back(glm::vec3(0.0f, 9.81f, 0.0f));
}

void Rocket::updatePosition(glm::dvec3 newPosition)
{
	position = newPosition;
}

void Rocket::updateRotation(glm::dvec3 newRotation)
{
	rotation = newRotation;
}

glm::dvec3 Rocket::getRotation()
{
	return rotation;
}

void Rocket::makeRocketGeometry()
{
	// top:
	addTriangle(
		point(0.0, 0.0, 0.01600001),        //0
		point(-0.001, -0.001, 0.01400001),  //1
		point(0.001, -0.001, 0.01400002),
		point(0.0, -1.0, 0.0)  //2
	);

	addTriangle(
		point(0.0, 0.0, 0.01600002),        //0
		point(0.001, -0.001, 0.01400003),   //2
		point(0.001, 0.001, 0.01400004),
		point(1.0, 0.0, 0.0)//3
	);

	addTriangle(
		point(0.0, 0.0, 0.01600003),		  //0
		point(0.001, 0.001, 0.01400005),    //3
		point(-0.001, 0.001, 0.01400006),
		point(0.0, 1.0, 0.0)
		//4
	);

	addTriangle(
		point(0.0, 0.0, 0.01600004),        //0
		point(-0.001, 0.001, 0.01400007),   //4
		point(-0.001, -0.001, 0.01400008),
		point(-1.0, 0.0, 0.0)//1
	);
	
	// side 1:
	addTriangle(
		point(-0.001, -0.001, 0.01400009),
		point(-0.001, -0.001, 0.002),
		point(0.001, -0.001, 0.01400009),
		point(0.0, -1.0, 0.0)
	);

	addTriangle(
		point(0.001, -0.001, 0.01400010),
		point(-0.001, -0.001, 0.002),
		point(0.001, -0.001, 0.002),
		point(0.0, -1.0, 0.0)
	);

	// side 2:
	addTriangle(
		point(-0.001, -0.001, 0.01400011),
		point(-0.001, 0.001, 0.002001),
		point(-0.001, 0.001, 0.01400011),
		point(-1.0, 0.0, 0.0)
	);

	addTriangle(
		point(-0.001, -0.001, 0.01400012),
		point(-0.001, -0.001, 0.002002),
		point(-0.001, 0.001, 0.002002),
		point(-1.0, 0.0, 0.0)
	);

	// side 3:
	addTriangle(
		point(0.001, -0.001, 0.01400013),
		point(0.001, 0.001, 0.002003),
		point(0.001, 0.001, 0.01400013),
		point(1.0, 0.0, 0.0)
	);

	addTriangle(
		point(0.001, -0.001, 0.01400014),
		point(0.001, -0.001, 0.002004),
		point(0.001, 0.001, 0.002004),
		point(1.0, 0.0, 0.0)
	);

	// side 4:
	addTriangle(
		point(-0.001, 0.001, 0.01400015),
		point(-0.001, 0.001, 0.002005),
		point(0.001, 0.001, 0.01400015),
		point(0.0, 1.0, 0.0)
	);

	addTriangle(
		point(0.001, 0.001, 0.01400016),
		point(-0.001, 0.001, 0.002006),
		point(0.001, 0.001, 0.002006),
		point(0.0, 1.0, 0.0)
	);
}

void Rocket::addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal)
{
	geometry->addTriangle(p1, p2, p3, normal);
}

glm::vec3 Rocket::point(double x, double y, double z)
{
	return glm::vec3(x, y, z);
}

Rocket::~Rocket()
{
	delete objectRenderer;
	delete geometry;
}