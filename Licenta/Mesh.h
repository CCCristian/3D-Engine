#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm.hpp"
#include "Assimp\include\assimp\Importer.hpp"
#include "Assimp\include\assimp\scene.h"
#include <vector>
#include "Material.h" 
#include "Utils.h"
#include "Shader.h"

namespace OpenGL
{
	class Mesh
	{
		friend class Renderer;
		struct Vertex;

		GLuint vao, vbo, ibo;
		int indexCount;
		//std::vector<Vertex> varfuri;
		//std::vector<unsigned> indici;
		Material *material;
	public:
		Mesh(const GLuint vao, const aiMesh* meshData, Material *material);
		Mesh(const GLuint vao, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Material *material);
		~Mesh();
		GLuint getHandle() const		{ return vao; }
		Material* getMaterial() const	{ return material; }
		int getIndexCount() const		{ return indexCount; }


	private:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 textureCoordinates;
			Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoordinates)
			{
				Vertex::position = position;
				Vertex::normal = normal;
				Vertex::textureCoordinates = textureCoordinates;
			}
		};
	};
}
