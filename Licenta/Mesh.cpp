#include "Mesh.h"
#include "Options.h"
#include "Debug.h"

namespace OpenGL
{
	const Mesh::VertexAttributeIndices Mesh::vertexAttributeIndices;

	Mesh::Mesh(const GLuint vao, const aiMesh* meshData, Material *material)
	{
		Mesh::vao = vao;
		std::vector<Vertex> varfuri;
		std::vector<unsigned> indici;

		varfuri.reserve(meshData->mNumVertices);
		for (unsigned i = 0; i < meshData->mNumVertices; i++)
		{
			glm::vec3 position(meshData->mVertices[i].x, meshData->mVertices[i].y, meshData->mVertices[i].z);

			glm::vec3 normal(0, 0, 0);
			if (meshData->HasNormals())
				normal = glm::vec3(meshData->mNormals[i].x, meshData->mNormals[i].y, meshData->mNormals[i].z);

			glm::vec2 texCoords(0, 0);
			if (meshData->mTextureCoords[0] != NULL)
				texCoords = glm::vec2(meshData->mTextureCoords[0][i].x, meshData->mTextureCoords[0][i].y);

			glm::vec3 tangent(0, 0, 0);
			if (meshData->HasTangentsAndBitangents())
				tangent = glm::vec3(meshData->mTangents[i].x, meshData->mTangents[i].y, meshData->mTangents[i].x);

			Vertex vertex(position, normal, texCoords, tangent);
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
		glVertexAttribPointer(vertexAttributeIndices.position,				3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, position));
		glVertexAttribPointer(vertexAttributeIndices.normal,				3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, normal));
		glVertexAttribPointer(vertexAttributeIndices.textureCoordinates,	2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, textureCoordinates));
		glVertexAttribPointer(vertexAttributeIndices.tangent,				3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, tangent));

		glEnableVertexAttribArray(vertexAttributeIndices.position);
		glEnableVertexAttribArray(vertexAttributeIndices.normal);
		glEnableVertexAttribArray(vertexAttributeIndices.textureCoordinates);
		glEnableVertexAttribArray(vertexAttributeIndices.tangent);


#ifdef USE_INSTANCED_RENDERING
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn1);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn2);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn3);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn4);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn1, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn2, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn3, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn4, 1);
#endif

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), &indici[0], GL_STATIC_DRAW);

		checkErrors();
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
		glVertexAttribPointer(vertexAttributeIndices.position,				3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, position));
		glVertexAttribPointer(vertexAttributeIndices.normal,				3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, normal));
		glVertexAttribPointer(vertexAttributeIndices.textureCoordinates,	2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, textureCoordinates));

		glEnableVertexAttribArray(vertexAttributeIndices.position);
		glEnableVertexAttribArray(vertexAttributeIndices.normal);
		glEnableVertexAttribArray(vertexAttributeIndices.textureCoordinates);


#ifdef USE_INSTANCED_RENDERING
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn1);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn2);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn3);
		glEnableVertexAttribArray(vertexAttributeIndices.instanceColumn4);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn1, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn2, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn3, 1);
		glVertexAttribDivisor(vertexAttributeIndices.instanceColumn4, 1);
#endif

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

		checkErrors();
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
