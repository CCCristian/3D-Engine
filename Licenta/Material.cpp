#include "Material.h"

namespace OpenGL
{
	const Material::MaterialKeysStruct Material::MaterialKeys;

	Material::Material()
	{
		Material::colorTexture = nullptr;
		Material::colorTexture2 = nullptr;
		Material::colorTexture3 = nullptr;
		Material::specularTexture = nullptr;
		Material::normalTexture = nullptr;
		Material::lightTexture = nullptr;
		//Material::heightTexture = nullptr;
		diffuseColor = glm::vec3(1);
		specularColor = glm::vec3(1);
		specularIntensity = 0.4f;
		specularPower = 20;
	}
	Material::Material(Texture *colorTexture, Texture *normalTexture): Material()
	{
		Material::colorTexture = colorTexture;
		Material::normalTexture = normalTexture;
	}
	Material::Material(Texture *colorTexture, Texture *colorTexture2, Texture *colorTexture3): Material()
	{
		Material::colorTexture = colorTexture;
		Material::colorTexture2 = colorTexture2;
		Material::colorTexture3 = colorTexture3;
	}
}
