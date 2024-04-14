#pragma once

#include <glad/glad.h>
#include "../../external_libraries/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <memory>

#include "Shader.h"
#include "Camera.h"

class SkyBoxRenderer
{
public:
    SkyBoxRenderer() { skyboxShader = std::make_unique<Shader>("shaders/skybox_vs.glsl", "shaders/skybox_fs.glsl"); }

	void init();

	void render(glm::dmat4& projection,
		glm::dmat4& view,
		ofsim_renderer::Camera* camera);

	~SkyBoxRenderer();

private:
    std::unique_ptr<Shader> skyboxShader;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	
	std::vector<std::string> faces;
	
	unsigned int loadCubemap();
};
