#include "pch.h"
#include "Model.h"
#include "Debug.h"

extern bool useNormalMapping;

namespace OpenGL
{
	Model* Model::quadModel = nullptr;

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
			aiString colorPath, normalPath;
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &colorPath);
			scene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &normalPath);
			Material::Builder builder(Texture::loadTexture(directory + colorPath.C_Str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR));
			builder.setNormalTexture(Texture::loadTexture(directory + normalPath.C_Str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR));
			aiColor3D col;
			for (unsigned j = 0; j < scene->mMaterials[i]->mNumProperties; j++)
			{
				if (Material::MaterialKeys.diffuseColor == scene->mMaterials[i]->mProperties[j]->mKey.C_Str())
				{
					scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, col);
					if (col.r != 0.0 || col.g != 0.0 || col.b != 0.0)
						builder.setDiffuseColor(glm::vec3(col.r, col.g, col.b));
				}
				else if (Material::MaterialKeys.specularColor == scene->mMaterials[i]->mProperties[j]->mKey.C_Str())
				{
					scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, col);
					builder.setSpecularColor(glm::vec3(col.r, col.g, col.b));
				}
				else if (Material::MaterialKeys.specularIntensity == scene->mMaterials[i]->mProperties[j]->mKey.C_Str())
				{
					float strength;
					scene->mMaterials[i]->Get(AI_MATKEY_SHININESS_STRENGTH, strength);
					builder.setSpecularIntensity(strength);
				}
				else if (Material::MaterialKeys.specularPower == scene->mMaterials[i]->mProperties[j]->mKey.C_Str())
				{
					float parameter;
					scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, parameter);
					builder.setSpecularPower(parameter);
				}
			}
			materials.push_back(builder.build());
		}

		// Incarcare meshes
		meshes.reserve(scene->mNumMeshes);
		vaos = new GLuint[scene->mNumMeshes];
		checkErrors();
		glGenVertexArrays(scene->mNumMeshes, vaos);
		checkErrors();
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
		materials.push_back(Material::Builder(Texture::loadTexture(texturePath)).build());
		meshes.push_back(new Mesh(*vaos, positions, normals, texCoords, indices, materials[0]));
		meshObjects.push_back(new MeshObject(glm::mat4(1), meshes[0]));
	}
	Model::Model(std::string name, const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned>& indices, Texture *tex)
	{
		Model::name = name;
		vaos = new GLuint;
		glGenVertexArrays(1, vaos);
		materials.push_back(Material::Builder(tex).build());
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

	void Model::prepareShader(int meshObjectIndex, const Shader& shader) const
	{
		if (shader.shaderType == Shader::ShaderType::MainShader)
		{
			Material* material = meshObjects[meshObjectIndex]->mesh->getMaterial();
			glm::vec3 color = material->getDiffuseColor();
			glUniform3f(SceneRenderingShader::uniformLocations.materialDiffuseColor, color.x, color.y, color.z);
			color = material->getSpecularColor();
			glUniform3f(SceneRenderingShader::uniformLocations.materialSpecularColor, color.x, color.y, color.z);
			glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularIntensity, material->getSpecularIntensity());
			glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularPower, material->getSpecularPower());
			glUniform1i(SceneRenderingShader::uniformLocations.textureCount, 1);
			glUniform1f(SceneRenderingShader::uniformLocations.textureRepeatCount, 1);
			glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.colorSampler);
			glBindTexture(GL_TEXTURE_2D, material->getColorTexture()->getHandle());
			if (material->hasNormalMap())
			{
				shader.loadUniform(SceneRenderingShader::uniformLocations.hasNormalMap, useNormalMapping && true);
				glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.normalSampler);
				glBindTexture(GL_TEXTURE_2D, material->getNormalTexture()->getHandle());
			}
			else
				shader.loadUniform(SceneRenderingShader::uniformLocations.hasNormalMap, useNormalMapping && false);
			checkErrors();
		}
	}

	const Model* Model::createModel(std::string name, std::string file)
	{
		const Model *mod = new Model(name, file);
		//modele.insert(std::make_pair(name, mod));
		return mod;
	}

	const Model* Model::getQuadModel()
	{
		if (quadModel == nullptr)
		{
			quadModel = BaseModelGenerator::generateQuad("Texture Quad", nullptr);
		}
		return quadModel;
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
	Model * Model::BaseModelGenerator::generateCube(std::string name, std::string texturePath, bool interiorNormals, float repeats)
	{
		return new Model(name, createCubeVertices(), createCubeNormals(interiorNormals), createCubeTexCoords(repeats), createCubeIndices(), texturePath);
	}
	Model * Model::BaseModelGenerator::generateCube(std::string name, Texture* tex, bool interiorNormals, float repeats)
	{
		return new Model(name, createCubeVertices(), createCubeNormals(interiorNormals), createCubeTexCoords(repeats), createCubeIndices(), tex);
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
	std::vector<glm::vec3> Model::BaseModelGenerator::createCubeVertices()
	{
		std::vector<glm::vec3> varfuri;

		// Front face
		varfuri.push_back(glm::vec3(-1,	-1,	-1));
		varfuri.push_back(glm::vec3(1,	-1,	-1));
		varfuri.push_back(glm::vec3(1,	-1,	1));
		varfuri.push_back(glm::vec3(-1,	-1,	1));

		// Back face
		varfuri.push_back(glm::vec3(1,	1,	-1));
		varfuri.push_back(glm::vec3(-1,	1,	-1));
		varfuri.push_back(glm::vec3(-1,	1,	1));
		varfuri.push_back(glm::vec3(1,	1,	1));

		// Left face
		varfuri.push_back(glm::vec3(-1,	1,	-1));
		varfuri.push_back(glm::vec3(-1,	-1,	-1));
		varfuri.push_back(glm::vec3(-1,	-1,	1));
		varfuri.push_back(glm::vec3(-1,	1,	1));

		// Right face
		varfuri.push_back(glm::vec3(1,	-1,	-1));
		varfuri.push_back(glm::vec3(1,	1,	-1));
		varfuri.push_back(glm::vec3(1,	1,	1));
		varfuri.push_back(glm::vec3(1,	-1,	1));

		// Up face
		varfuri.push_back(glm::vec3(-1,	-1,	1));
		varfuri.push_back(glm::vec3(1,	-1,	1));
		varfuri.push_back(glm::vec3(1,	1,	1));
		varfuri.push_back(glm::vec3(-1,	1,	1));

		// Down face
		varfuri.push_back(glm::vec3(-1,	1,	-1));
		varfuri.push_back(glm::vec3(1,	1,	-1));
		varfuri.push_back(glm::vec3(1,	-1,	-1));
		varfuri.push_back(glm::vec3(-1,	-1,	-1));

		return varfuri;
	}
	std::vector<glm::vec3> Model::BaseModelGenerator::createCubeNormals(bool interiorNormals)
	{
		std::vector<glm::vec3> normale;
		int orientare = 1;
		if (interiorNormals)
			orientare = -1;

		// Front face
		normale.push_back(glm::vec3(0, -1 * orientare, 0));
		normale.push_back(glm::vec3(0, -1 * orientare, 0));
		normale.push_back(glm::vec3(0, -1 * orientare, 0));
		normale.push_back(glm::vec3(0, -1 * orientare, 0));

		// Back face
		normale.push_back(glm::vec3(0, 1 * orientare, 0));
		normale.push_back(glm::vec3(0, 1 * orientare, 0));
		normale.push_back(glm::vec3(0, 1 * orientare, 0));
		normale.push_back(glm::vec3(0, 1 * orientare, 0));

		// Left face
		normale.push_back(glm::vec3(-1 * orientare, 0, 0));
		normale.push_back(glm::vec3(-1 * orientare, 0, 0));
		normale.push_back(glm::vec3(-1 * orientare, 0, 0));
		normale.push_back(glm::vec3(-1 * orientare, 0, 0));

		// Right face
		normale.push_back(glm::vec3(1 * orientare, 0, 0));
		normale.push_back(glm::vec3(1 * orientare, 0, 0));
		normale.push_back(glm::vec3(1 * orientare, 0, 0));
		normale.push_back(glm::vec3(1 * orientare, 0, 0));

		// Up face
		normale.push_back(glm::vec3(0, 0, 1 * orientare));
		normale.push_back(glm::vec3(0, 0, 1 * orientare));
		normale.push_back(glm::vec3(0, 0, 1 * orientare));
		normale.push_back(glm::vec3(0, 0, 1 * orientare));

		// Down face
		normale.push_back(glm::vec3(0, 0, -1 * orientare));
		normale.push_back(glm::vec3(0, 0, -1 * orientare));
		normale.push_back(glm::vec3(0, 0, -1 * orientare));
		normale.push_back(glm::vec3(0, 0, -1 * orientare));

		return normale;
	}
	std::vector<glm::vec2> Model::BaseModelGenerator::createCubeTexCoords(float repeats)
	{
		std::vector<glm::vec2> coordTexturare;

		// Front face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		// Back face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		// Left face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		// Right face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		// Up face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		// Down face
		coordTexturare.push_back(glm::vec2(0.0f, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, 0.0f));
		coordTexturare.push_back(glm::vec2(repeats, repeats));
		coordTexturare.push_back(glm::vec2(0.0f, repeats));

		return coordTexturare;
	}
	std::vector<unsigned> Model::BaseModelGenerator::createCubeIndices()
	{
		std::vector<unsigned> indici;
		for (int i = 0; i < 24; i += 4)
		{
			indici.push_back(i + 0); indici.push_back(i + 1); indici.push_back(i + 2);
			indici.push_back(i + 0); indici.push_back(i + 2); indici.push_back(i + 3);
		}
		return indici;
	}
}
