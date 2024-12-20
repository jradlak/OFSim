#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>

#include "Shader.h"
#include "Model.h"
#include "../math_and_physics/MathTypes.h"

namespace ofsim_renderer
{
    struct ModelRenderer
	{	
		ModelRenderer(std::string shaderName, std::string modelPath);

		void renderWithRotation(dmat4& projection,
						dmat4& view,
						dvec3 size,
						dvec3 position,
						dvec3 rotation = dvec3(0.0, 0.0, 0.0));

		void renderParallelToVector(dmat4& projection,
						dmat4& view,
						dvec3 size,
						const dvec3& position,
						const dvec3& direction);

		Shader* getShader() { return shader.get(); }

	private:
		std::unique_ptr<Shader> shader;
		std::unique_ptr<Model> objectModel;

		f64 logDepthBufFC;	

		// returns the rotation matrix that aligns the object to the given direction vector
		dmat4 alignToDirection(const dvec3& direction);
	};
}
