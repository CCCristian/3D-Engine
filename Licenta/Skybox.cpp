#include "pch.h"
#include "Skybox.h"
#include "Debug.h"

namespace OpenGL
{
	GLuint Skybox::vao, Skybox::vbo, Skybox::ibo;
	std::vector<glm::vec3> Skybox::varfuri;
	std::vector<unsigned> Skybox::indici;


	Skybox::Skybox(const char *right, const char *left, const char *front, const char *back, const char *up, const char *down)
	{
		glGenTextures(1, &cubemapHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapHandle);
		Texture::TextureData data1(right);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, data1.width, data1.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1.data);
		Texture::TextureData data2(left);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, data2.width, data2.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2.data);
		Texture::TextureData data3(front);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, data3.width, data3.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3.data);
		Texture::TextureData data4(back);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, data4.width, data4.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4.data);
		Texture::TextureData data5(up);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, data5.width, data5.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5.data);
		Texture::TextureData data6(down);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, data6.width, data6.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6.data);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		static int initialized = [this]
		{
			setupVAO();
			return 0;
		}();

		checkErrors();
	}
	Skybox::~Skybox()
	{
		glDeleteTextures(1, &cubemapHandle);
	}

	std::vector<glm::vec3> Skybox::createCubeVertices()
	{
		std::vector<glm::vec3> varfuri;

		varfuri.push_back(glm::vec3(-1, -1, -1));
		varfuri.push_back(glm::vec3(1, -1, -1));
		varfuri.push_back(glm::vec3(1, 1, -1));
		varfuri.push_back(glm::vec3(-1, 1, -1));
		varfuri.push_back(glm::vec3(-1, -1, 1));
		varfuri.push_back(glm::vec3(1, -1, 1));
		varfuri.push_back(glm::vec3(1, 1, 1));
		varfuri.push_back(glm::vec3(-1, 1, 1));

		return varfuri;
	}
	std::vector<unsigned> Skybox::createCubeIndices()
	{
		std::vector<unsigned> indici;

		// Right face
		indici.push_back(2);  indici.push_back(1);  indici.push_back(5);
		indici.push_back(2);  indici.push_back(5);  indici.push_back(6);

		// Left face
		indici.push_back(0);  indici.push_back(3);  indici.push_back(7);
		indici.push_back(0);  indici.push_back(7);  indici.push_back(4);

		// Front face
		indici.push_back(3);  indici.push_back(2);  indici.push_back(6);
		indici.push_back(3);  indici.push_back(6);  indici.push_back(7);

		// Back face
		indici.push_back(0);  indici.push_back(5);  indici.push_back(1);
		indici.push_back(0);  indici.push_back(4);  indici.push_back(5);

		// Up face
		indici.push_back(4);  indici.push_back(6);  indici.push_back(5);
		indici.push_back(4);  indici.push_back(7);  indici.push_back(6);

		// Down face
		indici.push_back(0);  indici.push_back(1);  indici.push_back(2);
		indici.push_back(0);  indici.push_back(2);  indici.push_back(3);

		return indici;
	}
	void Skybox::setupVAO()
	{
		varfuri = createCubeVertices();
		indici = createCubeIndices();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, varfuri.size() * sizeof(glm::vec3), &varfuri[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indici.size() * sizeof(unsigned), &indici[0], GL_STATIC_DRAW);
	}
}
