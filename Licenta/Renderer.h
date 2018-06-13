#pragma once

#include "Object.h"
#include "ModelInstanceData.h"
#include "Skybox.h"
#include "Water.h"

namespace OpenGL
{
	class Renderer
	{
		GLuint singleMatrixVBO;
		Shader::ShaderType activeShader;
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		~Renderer();
		void draw(const Object& object);
		void draw(const Water& water);
		void draw(const ModelInstanceData &data, bool drawForShadowMap = false);
		void draw(Skybox* skybox, glm::mat4& world);
		void useShader(const Shader& shader);
	};
}
