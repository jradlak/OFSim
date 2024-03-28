#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(std::string shaderName, bool _textured) : textured(_textured)
{
	shader = new Shader(("shaders/" + shaderName + "_vs.glsl").c_str(), ("shaders/" + shaderName + "_fs.glsl").c_str());
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
			
	if (textured)
	{
		// 6 * sizeof(float)
		//position attribute:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//normal attribute:
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	else
	{
		//position attribute:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//normal attribute:
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

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
	const glm::dvec3& position, 
	glm::dvec3 rotationAxis,
	double rotationAngle)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	model = glm::translate(model, position);

	// calculate rotations:
	if (rotationAngle != 0.0)
	{		
		model = glm::rotate(model, rotationAngle, rotationAxis);
	}
	
	model = glm::scale(model, glm::dvec3(size));
	shader->setMat4("model", glm::mat4(model));

	glm::mat4 transformation = glm::mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ObjectRenderer::renderWithRotation(glm::dmat4& projection,
	glm::dmat4& view,
	double size,
	const glm::dvec3& position,
	glm::dvec3 rotation)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	model = glm::translate(model, position);

	// calculate rotations:
	
	model = glm::rotate(model, glm::radians(rotation.x), glm::dvec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::dvec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::dvec3(0.0, 0.0, 1.0));
	
	model = glm::scale(model, glm::dvec3(size));
	shader->setMat4("model", glm::mat4(model));

	glm::mat4 transformation = glm::mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	if (textured)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ObjectRenderer::loadTexture(std::string fileName)
{
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	shader->use();
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(shader->ID, "texture1"), 0);
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