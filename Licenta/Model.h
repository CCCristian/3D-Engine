#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm.hpp"
#include "glm\gtc\constants.hpp"
#include "Assimp\include\assimp\Importer.hpp"
#include "Assimp\include\assimp\scene.h"
#include <set>
#include <map>
#include <vector>
#include "Texture.h"
#include "Utils.h"
#include "Mesh.h"
#include "Shader.h"

namespace OpenGL
{
	class Model
	{
	public:
		struct MeshObject
		{
			glm::mat4 transform;
			Mesh* mesh;
			MeshObject(glm::mat4 transform, Mesh *mesh): transform(transform), mesh(mesh) {  }
		};


	private:
		std::string name;
		void constructMeshObjects(aiNode *node, aiMatrix4x4 transform);
		glm::mat4 convertToGLMMat4(aiMatrix4x4 &mat);
		static Model* quadModel;
	protected:
		Model(std::string name, int vaosCount);
		GLuint* vaos;
		std::vector<Mesh *> meshes;
		std::vector<MeshObject *> meshObjects;
		std::vector<Material *> materials;
	public:
		static const Model* createModel(std::string name, std::string file);
		static const Model* getQuadModel();
		Model(std::string name, std::string file);
		Model(std::string name, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, std::string texturePath);
		Model(std::string name, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Texture *tex);
		Model(const Model&) = delete;
		virtual ~Model();
		/** Upload all necessary non-instance uniforms to the shader, bind the VAO, bind the appropriate textures, etc. */
		virtual void prepareShader(int meshObjectIndex, const Shader& shader) const;
		int getMeshCount() const									{ return meshes.size(); }
		const std::vector<Mesh *>& getMeshes() const				{ return meshes; }
		int getMeshObjectsCount() const								{ return meshObjects.size(); }
		const std::vector<MeshObject *>& getMeshObjects() const		{ return meshObjects; }
		const std::vector<Material *>& getMaterials() const			{ return materials; }




		class BaseModelGenerator
		{
		public:
			static Model* generateQuad(std::string name, std::string texturePath, float repeats = 1);
			static Model* generateQuad(std::string name, Texture *tex, float repeats = 1);

			static Model* generateCube(std::string name, std::string texturePath, bool interiorNormals = false, float repeats = 1);
			static Model* generateCube(std::string name, Texture *tex, bool interiorNormals = false, float repeats = 1);

		private:
			static std::vector<glm::vec3> createQuadVertices();
			static std::vector<glm::vec3> createQuadNormals();
			static std::vector<glm::vec2> createQuadTexCoords(float repeats);
			static std::vector<unsigned> createQuadIndices();

			static std::vector<glm::vec3> createCubeVertices();
			static std::vector<glm::vec3> createCubeNormals(bool interiorNormals = false);
			static std::vector<glm::vec2> createCubeTexCoords(float repeats);
			static std::vector<unsigned> createCubeIndices();
		};
	};
}
