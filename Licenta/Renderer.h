#pragma once

#include "Object.h"
#include "ModelInstanceData.h"
#include "Skybox.h"

namespace OpenGL
{
	class Renderer
	{
		GLuint singleMatrixVBO;
	public:
		Renderer();
		~Renderer();
		void draw(const Object& object);
		void draw(const ModelInstanceData &data, bool drawForShadowMap = false);
		void draw(Skybox* skybox, glm::mat4& world);
	};
}
