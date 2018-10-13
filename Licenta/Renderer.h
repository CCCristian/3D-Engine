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
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		~Renderer();
		void draw(const Shader& shader, const Object& object);
		void draw(const Shader& shader, const Water& water);
		void draw(const Shader& shader, const ModelInstanceData &data);
		void draw(const Shader& shader, const Skybox* skybox, const glm::mat4& world);
		void updateVBOsToGPU(const ModelInstanceData& data) const;
	};
}
