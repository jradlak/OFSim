#pragma once

#include <glad/glad.h>
#include "../../external_libraries/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		ofsim_renderer::Camera* camera);

	~SkyBoxRenderer();

private:
	Shader* skyboxShader;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
	std::vector<std::string> faces;
	
	unsigned int loadCubemap();
};