#include "Mesh.h"
#include "Options.h"
#include "Error.h"

namespace OpenGL
{
	Mesh::Mesh(const GLuint vao, const aiMesh* meshData, Material *material)
	{
		Mesh::vao = vao;
		std::vector<Vertex> varfuri;
		std::vector<unsigned> indici;

		varfuri.reserve(meshData->mNumVertices);
		for (unsigned i = 0; i < meshData->mNumVertices; i++)
		{
			glm::vec3 position(meshData->mVertices[i].x, meshData->mVertices[i].y, meshData->mVertices[i].z);
			glm::vec3 normal(meshData->mNormals[i].x, meshData->mNormals[i].y, meshData->mNormals[i].z);
			glm::vec2 texCoords;
			if (meshData->mTextureCoords[0] != NULL)
				texCoords = glm::vec2(meshData->mTextureCoords[0][i].x, meshData->mTextureCoords[0][i].y);
			else
				texCoords = glm::vec2(0, 0);
			Vertex vertex(position, normal, texCoords);
			varfuri.push_back(vertex);
		}

		indici.reserve(3 * meshData->mNumFaces);
		for (unsigned i = 0; i < meshData->mNumFaces; i++)
			for (unsigned j = 0; j < meshData->mFaces[i].mNumIndices; j++)
				indici.push_back(meshData->mFaces[i].mIndices[j]);
		indexCount = indici.size();

		Mesh::material = material;


		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, varfuri.size() * sizeof(Vertex), &varfuri[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, normal));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, textureCoordinates));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
#ifdef USE_INSTANCED_RENDERING
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
#endif

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), &indici[0], GL_STATIC_DRAW);

#ifdef _DEBUG
		checkErrors();
#endif
	}

	Mesh::Mesh(const GLuint vao, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Material *material)
	{
		Mesh::vao = vao;
		std::vector<Vertex> varfuri;
		indexCount = indices.size();

		varfuri.reserve(positions.size());
		for (unsigned i = 0; i < positions.size(); i++)
			varfuri.push_back(Vertex(positions[i], normals[i], texCoords[i]));
		Mesh::material = material;


		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, varfuri.size() * sizeof(Vertex), &varfuri[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, normal));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, textureCoordinates));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
#ifdef USE_INSTANCED_RENDERING
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
#endif

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

#ifdef _DEBUG
		checkErrors();
#endif
	}

	Mesh::~Mesh()
	{
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &ibo);
		glDeleteBuffers(1, &vbo);
		//glBindVertexArray(0);
	}
}
