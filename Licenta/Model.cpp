#include "Model.h"
#include <iostream>
#include "assimp\postprocess.h"
#include "glm\gtc\matrix_transform.hpp"

namespace OpenGL
{
	Mesh* Model::quadMesh = nullptr;

	Model::Model(std::string name, int vaosCount)
	{
		Model::name = name;
		vaos = new GLuint[vaosCount];
		glGenVertexArrays(vaosCount, vaos);
	}

	Model::Model(std::string name, std::string file)
	{
		Model::name = name;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file, aiProcessPreset_TargetRealtime_Quality | aiProcess_GenNormals);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Eroare la incarcarea modelului: " << file << '\n';
			std::cin.get();
			exit(1);
		}
		std::string directory = getDirectory(file);

		// Incarcare materiale
		for (unsigned i = 0; i < scene->mNumMaterials; i++)
		{
			aiString path;
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			Material *mat = new Material(Texture::loadTexture(directory + path.C_Str()));
			aiColor3D col;
			for (unsigned j = 0; j < scene->mMaterials[i]->mNumProperties; j++)
			{
				if (scene->mMaterials[i]->mProperties[j]->mKey.C_Str() == Material::MaterialKeys.diffuseColor)
				{
					scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, col);
					if (col.r != 0.0 || col.g != 0.0 || col.b != 0.0)
						mat->diffuseColor = glm::vec3(col.r, col.g, col.b);
				}
				else if (scene->mMaterials[i]->mProperties[j]->mKey.C_Str() == Material::MaterialKeys.specularColor)
				{
					scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, col);
					mat->specularColor = glm::vec3(col.r, col.g, col.b);
				}
				else if (scene->mMaterials[i]->mProperties[j]->mKey.C_Str() == Material::MaterialKeys.specularIntensity)
				{
					scene->mMaterials[i]->Get(AI_MATKEY_SHININESS_STRENGTH, mat->specularIntensity);
				}
				else if (scene->mMaterials[i]->mProperties[j]->mKey.C_Str() == Material::MaterialKeys.specularPower)
				{
					float parameter;
					scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, parameter);
					if (parameter >= 2)
						mat->specularPower = parameter;
				}
			}
			materials.push_back(mat);
		}

		// Incarcare meshes
		meshes.reserve(scene->mNumMeshes);
		vaos = new GLuint[scene->mNumMeshes];
		glGenVertexArrays(scene->mNumMeshes, vaos);
		for (unsigned i = 0; i < scene->mNumMeshes; i++)
		{
			meshes.push_back(new Mesh(vaos[i], scene->mMeshes[i], materials[scene->mMeshes[i]->mMaterialIndex]));
		}

		// Incarcare obiecte
		constructMeshObjects(scene->mRootNode, scene->mRootNode->mTransformation);

		importer.FreeScene();
	}

	Model::Model(std::string name, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, std::string texturePath)
	{
		Model::name = name;
		vaos = new GLuint;
		glGenVertexArrays(1, vaos);
		materials.push_back(new Material(Texture::loadTexture(texturePath)));
		meshes.push_back(new Mesh(*vaos, positions, normals, texCoords, indices, materials[0]));
		meshObjects.push_back(new MeshObject(glm::mat4(1), meshes[0]));
	}
	Model::Model(std::string name, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Texture *tex)
	{
		Model::name = name;
		vaos = new GLuint;
		glGenVertexArrays(1, vaos);
		materials.push_back(new Material(tex));
		meshes.push_back(new Mesh(*vaos, positions, normals, texCoords, indices, materials[0]));
		meshObjects.push_back(new MeshObject(glm::mat4(1), meshes[0]));
	}

	Model::~Model()
	{
		glDeleteVertexArrays(meshes.size(), vaos);
		for (Mesh* mesh : meshes)
			delete mesh;
		for (MeshObject* meshObject : meshObjects)
			delete meshObject;
		for (Material* material : materials)
			delete material;
		delete[] vaos;
	}

	void Model::prepareShader(int meshObjectIndex) const
	{
		Material* material = meshObjects[meshObjectIndex]->mesh->getMaterial();
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.colorSampler);
		glBindTexture(GL_TEXTURE_2D, material->colorTexture->getHandle());
		glUniform3f(SceneRenderingShader::uniformLocations.materialDiffuseColor, material->diffuseColor.x, material->diffuseColor.y, material->diffuseColor.z);
		glUniform3f(SceneRenderingShader::uniformLocations.materialSpecularColor, material->specularColor.x, material->specularColor.y, material->specularColor.z);
		glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularIntensity, material->specularIntensity);
		glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularPower, material->specularPower);
		glUniform1i(SceneRenderingShader::uniformLocations.textureCount, 1);
		glUniform1f(SceneRenderingShader::uniformLocations.textureRepeatCount, 1);
	}

	const Model* Model::createModel(std::string name, std::string file)
	{
		const Model *mod = new Model(name, file);
		//modele.insert(std::make_pair(name, mod));
		return mod;
	}

	const Mesh* Model::getQuadVAO()
	{
		if (quadMesh == nullptr)
		{
			quadMesh = BaseModelGenerator::generateQuad("Texture Quad", nullptr)->getMeshes()[0];
		}
		return quadMesh;
	}

	void Model::constructMeshObjects(aiNode *node, aiMatrix4x4 transform)
	{
		for (unsigned i = 0; i < node->mNumMeshes; i++)
			meshObjects.push_back(new MeshObject(glm::transpose(convertToGLMMat4(transform)), meshes[node->mMeshes[i]]));

		for (unsigned i = 0; i < node->mNumChildren; i++)
			constructMeshObjects(node->mChildren[i], transform * node->mChildren[i]->mTransformation);
	}

	glm::mat4 Model::convertToGLMMat4(aiMatrix4x4 &mat)
	{
		glm::vec4 v1(mat.a1, mat.b1, mat.c1, mat.d1);
		glm::vec4 v2(mat.a2, mat.b2, mat.c2, mat.d2);
		glm::vec4 v3(mat.a3, mat.b3, mat.c3, mat.d3);
		glm::vec4 v4(mat.a4, mat.b4, mat.c4, mat.d4);
		return glm::mat4(v1, v2, v3, v4);
	}

	// **************************************************************
	// **************************************************************
	// **************************************************************

	Model* Model::BaseModelGenerator::generateQuad(std::string name, std::string texturePath, float repeats)
	{
		return new Model(name, createQuadVertices(), createQuadNormals(), createQuadTexCoords(repeats), createQuadIndices(), texturePath);
	}
	Model* Model::BaseModelGenerator::generateQuad(std::string name, Texture *tex, float repeats)
	{
		return new Model(name, createQuadVertices(), createQuadNormals(), createQuadTexCoords(repeats), createQuadIndices(), tex);
	}
	std::vector<glm::vec3> Model::BaseModelGenerator::createQuadVertices()
	{
		std::vector<glm::vec3> varfuri;
		varfuri.push_back(glm::vec3(-1,		-1,		0));
		varfuri.push_back(glm::vec3(1,		-1,		0));
		varfuri.push_back(glm::vec3(1,		1,		0));
		varfuri.push_back(glm::vec3(-1,		1,		0));
		return varfuri;
	}
	std::vector<glm::vec3> Model::BaseModelGenerator::createQuadNormals()
	{
		std::vector<glm::vec3> normale;
		normale.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		normale.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		normale.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		normale.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		return normale;
	}
	std::vector<glm::vec2> Model::BaseModelGenerator::createQuadTexCoords(float repeats)
	{
		std::vector<glm::vec2> coordTexturare;
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));
		return coordTexturare;
	}
	std::vector<unsigned> Model::BaseModelGenerator::createQuadIndices()
	{
		std::vector<unsigned> indici;
		indici.push_back(0); indici.push_back(1); indici.push_back(2);
		indici.push_back(0); indici.push_back(2); indici.push_back(3);
		return indici;
	}
}