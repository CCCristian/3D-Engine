#pragma once

#include "GL.h"
#include "Material.h" 
#include "Utils.h"
#include "Shader.h"

namespace OpenGL
{
	class Mesh
	{
		struct Vertex;
		struct VertexAttributeIndices;

		static const VertexAttributeIndices vertexAttributeIndices;

		GLuint vao, vbo, ibo;
		int indexCount;
		Material *material;
	public:
		Mesh(const GLuint vao, const aiMesh* meshData, Material *material);
		Mesh(const GLuint vao, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Material *material);
		Mesh(const Mesh&) = delete;
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
			glm::vec3 tangent;
			Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoordinates, glm::vec3 tangent = glm::vec3(0))
			{
				Vertex::position = position;
				Vertex::normal = normal;
				Vertex::textureCoordinates = textureCoordinates;
				Vertex::tangent = tangent;
			}
		};
		struct VertexAttributeIndices
		{
			const int position = 0;
			const int color = 1;
			const int normal = 2;
			const int textureCoordinates = 3;
			const int tangent = 4;

			const int instanceColumn1 = 5;
			const int instanceColumn2 = 6;
			const int instanceColumn3 = 7;
			const int instanceColumn4 = 8;
		};
	};
}
