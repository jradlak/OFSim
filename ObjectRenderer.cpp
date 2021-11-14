#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(std::string shaderName)
{
	shader = new Shader((shaderName + "_vs.glsl").c_str(), (shaderName + "_fs.glsl").c_str());
	logDepthBufFC = 2.0 / log(150000000000000000.0 + 1.0) / 0.69315;
}

void ObjectRenderer::init(std::vector<float> _vertices, std::vector<int> _indices)
{
	std::vector<float> vert = _vertices;
	int numOfVertices = vert.size();
	float* vertices = new float[numOfVertices];
	std::copy(vert.begin(), vert.end(), vertices);
	
	std::vector<int> ind = _indices;
	int numOfIndices = ind.size();
	int* indices = new int[numOfIndices];
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

	delete[] vertices;
	delete[] indices;
}

Shader* ObjectRenderer::getShader()
{
	return shader;
}

void ObjectRenderer::render(glm::dmat4& projection, glm::dmat4& view,
	double size,
	glm::dvec3 position, 
	glm::vec3 rotation)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	model = glm::translate(model, position);

	// calculate rotations:
	if (rotation.x != 0.0f)
	{
		model = glm::rotate(model, (double)glm::radians(rotation.x), glm::dvec3(1.0, 0.0, 0.0));
	}
	
	if (rotation.y != 0.0f)
	{
		model = glm::rotate(model, (double)glm::radians(rotation.y), glm::dvec3(0.0, 1.0, 0.0));
	}

	if (rotation.z != 0.0f)
	{
		model = glm::rotate(model, (double)glm::radians(rotation.z), glm::dvec3(0.0, 0.0, 1.0));
	}

	model = glm::scale(model, glm::dvec3(size));
	shader->setMat4("model", glm::mat4(model));

	glm::mat4 transformation = glm::mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ObjectRenderer::clear()
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

ObjectRenderer::~ObjectRenderer()
{
	delete shader;
	clear();
}