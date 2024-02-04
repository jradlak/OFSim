#pragma once

#include "../../external_libraries/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Model.h"
#include "../math_and_physics/MathTypes.h"

namespace ofsim_renderer
{
	class ModelRenderer
	{
	public:
		ModelRenderer(std::string shaderName, std::string modelPath);

		void renderWithRotation(dmat4& projection,
						dmat4& view,
						f64 size,
						dvec3 position,
						dvec3 rotation = dvec3(0.0, 0.0, 0.0));

		Shader* getShader() { return shader.get(); }

		~ModelRenderer() {}

	private:
		std::unique_ptr<Shader> shader;
		std::unique_ptr<Model> objectModel;

		f64 logDepthBufFC;	
	};
}