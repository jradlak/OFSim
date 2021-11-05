#include "CelestialBody.h"

CelestialBody::CelestialBody(CelestialBodyType _bodyType, std::string shaderName,
	float _size, glm::vec3& _position) : position(_position)
{	
	bodyType = _bodyType;
	size = _size;
	indexCount = 0;

	sphere = new Sphere(0.5f, 32, 16);
	shader = new Shader((shaderName + "_vs.glsl").c_str(), (shaderName + "_fs.glsl").c_str());
}

void CelestialBody::init()
{
	std::vector<float> vert = sphere->getVertices();
	float vertices[3366];
	int numOfVertices = vert.size();
	std::copy(vert.begin(), vert.end(), vertices);

	std::vector<int> ind = sphere->getIndices();
	int indices[2880];
	int numOfIndices = ind.size();
	std::copy(ind.begin(), ind.end(), indices);
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	//position attribute:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attribute:
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CelestialBody::render(glm::mat4& projection, glm::mat4& view, glm::vec3& _lightPos)
{
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	if (bodyType != star)
	{
		shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader->setVec3("lightPos", _lightPos);
	}

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(size));
	shader->setMat4("model", model);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CelestialBody::clear()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

CelestialBody::~CelestialBody()
{
	delete sphere;
	delete shader;

	clear();
}