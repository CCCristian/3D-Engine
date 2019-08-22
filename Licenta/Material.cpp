#include "pch.h"
#include "Material.h"

namespace OpenGL
{
	const Material::MaterialKeysStruct Material::MaterialKeys;
	const glm::vec3 Material::defaultDiffuseColor(1);
	const glm::vec3 Material::defaultSpecularColor(1);

	Material::Builder::Builder(const Texture* colorTexture)
	{
		diffuseColor = defaultDiffuseColor;
		specularColor = defaultSpecularColor;
		specularIntensity = defaultSpecularIntensity;
		specularPower = defaultSpecularPower;
		if (colorTexture != nullptr)
			Builder::colorTexture = colorTexture;
		else
			Builder::colorTexture = Texture::getDefaultTexture();
		colorTexture2 = nullptr;
		colorTexture3 = nullptr;
		normalTexture = nullptr;
		specularTexture = nullptr;
		//Material::lightTexture = nullptr;
		//Material::heightTexture = nullptr;
	}
	Material* Material::Builder::build()
	{
		Material *mat = new Material();
		mat->diffuseColor = diffuseColor;
		mat->specularColor = specularColor;
		mat->specularIntensity = specularIntensity;
		mat->specularPower = specularPower;
		mat->colorTexture = colorTexture;
		mat->colorTexture2 = colorTexture2;
		mat->colorTexture3 = colorTexture3;
		mat->normalTexture = normalTexture;
		mat->specularTexture = specularTexture;
		return mat;
	}
	Material::Builder& Material::Builder::setDiffuseColor(const glm::vec3& diffuseColor)
	{
		Builder::diffuseColor = diffuseColor;
		return *this;
	}
	Material::Builder& Material::Builder::setSpecularColor(const glm::vec3& specularColor)
	{
		Builder::specularColor = specularColor;
		return *this;
	}
	Material::Builder& Material::Builder::setSpecularIntensity(float specularIntensity)
	{
		Builder::specularIntensity = specularIntensity;
		return *this;
	}
	Material::Builder& Material::Builder::setSpecularPower(float specularPower)
	{
		if (specularPower > 2)
			Builder::specularPower = specularPower;
		return *this;
	}
	Material::Builder& Material::Builder::setColorTexture(const Texture* colorTexture)
	{
		if (colorTexture != nullptr)
			Builder::colorTexture = colorTexture;
		else
		{
			std::cerr << "Null texture pointer error.";
			std::cin.get();
		}
		return *this;
	}
	Material::Builder& Material::Builder::setColorTexture2(const Texture* colorTexture2)
	{
		Builder::colorTexture2 = colorTexture2;
		return *this;
	}
	Material::Builder& Material::Builder::setColorTexture3(const Texture* colorTexture3)
	{
		Builder::colorTexture3 = colorTexture3;
		return *this;
	}
	Material::Builder& Material::Builder::setNormalTexture(const Texture* normalTexture)
	{
		Builder::normalTexture = normalTexture;
		return *this;
	}
	Material::Builder& Material::Builder::setSpecularTexture(const Texture* specularTexture)
	{
		Builder::specularTexture = specularTexture;
		return *this;
	}

}
