#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <vector>
#include <string>

#include "Shader.h"
#include "Camera.h"

class SkyBoxRenderer
{
public:
	SkyBoxRenderer();

	void init();

	void render(glm::dmat4& projection,
		glm::dmat4& view,
		Camera* camera);

	~SkyBoxRenderer();

private:
	Shader* skyboxShader;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
	std::vector<std::string> faces;
	
	unsigned int loadCubemap();
};