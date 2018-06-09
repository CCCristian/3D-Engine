#pragma once

#include "Texture.h"
#include "Shader.h"
#include <vector>

namespace OpenGL
{
	class Skybox
	{
		friend class Renderer;
		GLuint cubemapHandle;
		static GLuint vao, vbo, ibo;
		static std::vector<glm::vec3> varfuri;
		static std::vector<unsigned> indici;
		static std::vector<glm::vec3> createCubeVertices();
		static std::vector<unsigned> createCubeIndices();
		static void setupVAO();
	public:
		Skybox(const char *right, const char *left, const char *front, const char *back, const char *up, const char *down);
		~Skybox();
		GLuint getVAO()			{ return vao; }
		GLuint getHandle()		{ return cubemapHandle; }
	};
}
