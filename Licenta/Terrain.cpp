#include "pch.h"
#include "Terrain.h"
#include "Debug.h"

namespace OpenGL
{
	Terrain::Terrain(std::string name, std::string heightMapFile, Texture* texture1, Texture* texture2, Texture* texture3, float maxHeight, Material* material): Model(name, 1), textureCount(1), textureRepeatCount(1)
	{
		int width, height;
		unsigned char *image = stbi_load(heightMapFile.c_str(), &width, &height, 0, heightMapChannelCount);
		if (image == 0)
		{
			std::cerr << "Eroare la incarcarea texturii height map: \"" << heightMapFile << "\"\n";
			exit(1);
		}

		// Compute positions
		std::vector<glm::vec3> positions;
		positions.reserve(width * height);
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				positions.push_back(glm::vec3(-1 + (float)i * 2 / width, -1 + (float)j * 2 / height, maxHeight*getHeightMapFactor(image, width, i, j)));

		// Compute texture coordinates
		std::vector<glm::vec2> textureCoordinates;
		textureCoordinates.reserve(width * height);
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				textureCoordinates.push_back(glm::vec2((float)i / width, (float)j / height));

		// Initialize normals
		std::vector<glm::vec3> normals;
		normals.reserve(width * height);
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				normals.push_back(glm::vec3(0));

		// Compute indices and add normals
		std::vector<unsigned> indices;
		indices.reserve((width - 1) * (height - 1) * 6);
		for (int i = 0; i < width-1; i++)
			for (int j = 0; j < height - 1; j++)
			{
				int i1 = i * width + j;
				int i2 = (i + 1) * width + j + 1;
				int i3 = i * width + j + 1; 
				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
				glm::vec3 normal = glm::normalize(glm::cross(positions[i1] - positions[i2], positions[i2] - positions[i3]));
				normals[i1] += normal;
				normals[i2] += normal;
				normals[i3] += normal;

				i1 = i * width + j;
				i2 = (i + 1) * width + j;
				i3 = (i + 1) * width + j + 1; 
				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
				normal = glm::normalize(glm::cross(positions[i1] - positions[i2], positions[i2] - positions[i3]));
				normals[i1] = normals[i1] + normal;
				normals[i2] = normals[i2] + normal;
				normals[i3] = normals[i3] + normal;
			}

		// Normalize normals
		for (int i = 0; i < width * height; i++)
				glm::normalize(normals[i]);

		materials.push_back(Material::Builder(texture1).setColorTexture2(texture2).setColorTexture3(texture3).build());
		meshes.push_back(new Mesh(*vaos, positions, normals, textureCoordinates, indices, materials[0]));
		meshObjects.push_back(new MeshObject(glm::mat4(1), meshes[0]));
		stbi_image_free(image);
	}

	Terrain::~Terrain()
	{

	}

	void Terrain::prepareShader(int meshObjectIndex, Shader::ShaderType shaderType) const
	{
		if (shaderType == Shader::ShaderType::MainShader)
		{
			Material* material = meshObjects[meshObjectIndex]->mesh->getMaterial();
			glm::vec3 color = material->getDiffuseColor();
			glUniform3f(SceneRenderingShader::uniformLocations.materialDiffuseColor, color.x, color.y, color.z);
			color = material->getSpecularColor();
			glUniform3f(SceneRenderingShader::uniformLocations.materialSpecularColor, color.x, color.y, color.z);
			glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularIntensity, material->getSpecularIntensity());
			glUniform1f(SceneRenderingShader::uniformLocations.materialSpecularPower, material->getSpecularPower());
			glUniform1i(SceneRenderingShader::uniformLocations.textureCount, textureCount);
			glUniform1f(SceneRenderingShader::uniformLocations.textureRepeatCount, textureRepeatCount);
			glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.colorSampler);
			glBindTexture(GL_TEXTURE_2D, material->getColorTexture()->getHandle());
			if (textureCount >= 2)
			{
				glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.colorSampler2);
				glBindTexture(GL_TEXTURE_2D, material->getColorTexture2()->getHandle());
			}
			if (textureCount >= 3)
			{
				glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.colorSampler3);
				glBindTexture(GL_TEXTURE_2D, material->getColorTexture3()->getHandle());
			}
		}
		checkErrors();
	}

	float Terrain::getHeightMapFactor(unsigned char* data, int width, int x, int y)
	{
		unsigned char pixelValue = data[width*x*heightMapChannelCount + y * heightMapChannelCount];
		return (float)pixelValue / 256;
	}



	Terrain::Builder::Builder()
	{
		name = "";
		heightMapFile = "";
		texture1 = nullptr;
		texture2 = nullptr;
		texture3 = nullptr;
		maxHeight = 10.f;
		textureRepeatCount = 1;

		addedTexturesCount = 0;
	}
	Terrain::Builder& Terrain::Builder::setName(std::string name)
	{
		Builder::name = name;
		return *this;
	}
	Terrain::Builder& Terrain::Builder::setHeightMapFile(std::string heightMapFile)
	{
		Builder::heightMapFile = heightMapFile;
		return *this;
	}
	Terrain::Builder& Terrain::Builder::addTexture(std::string texture)
	{
		if (++addedTexturesCount == 1)
			Builder::texture1 = Texture::loadTexture(texture, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		else if (++addedTexturesCount == 2)
			Builder::texture2 = Texture::loadTexture(texture, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		else if (++addedTexturesCount == 3)
			Builder::texture3 = Texture::loadTexture(texture, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		else
		{
			std::cout << "Eroare la constructia terenului - s-au adaugat prea multe texturi.";
			std::cin.get();
			exit(1);
		}
		return *this;
	}
	Terrain::Builder& Terrain::Builder::setMaxHeight(float maxHeight)
	{
		Builder::maxHeight = maxHeight;
		return *this;
	}
	Terrain::Builder& Terrain::Builder::setTextureRepeatCount(float textureRepeatCount)
	{
		Builder::textureRepeatCount = textureRepeatCount;
		return *this;
	}
	Terrain* Terrain::Builder::build()
	{
		if (addedTexturesCount == 0)
		{
			texture1 = Texture::getDefaultTexture();
			addedTexturesCount = 1;
		}
		Material::Builder builder(texture1);
		builder.setColorTexture2(texture2);
		builder.setColorTexture3(texture3);
		Terrain* terrain = new Terrain(name, heightMapFile, texture1, texture2, texture3, maxHeight, builder.build());
		terrain->textureCount = addedTexturesCount;
		terrain->textureRepeatCount = textureRepeatCount;
		return terrain;
	}
}
