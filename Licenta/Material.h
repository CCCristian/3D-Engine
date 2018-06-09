#pragma once

#include "Texture.h"


namespace OpenGL
{
	struct Material
	{
	private:
		struct MaterialKeysStruct
		{
			std::string diffuseColor = "$clr.diffuse";
			std::string specularColor = "$clr.specular";
			std::string specularIntensity = "$mat.shinpercent";
			std::string specularPower = "$mat.shininess";
		};

		/** For data initializations. */
		Material();
	public:
		/** Numele proprietatilor materialelor importate de Assimp. */
		static const MaterialKeysStruct MaterialKeys;


		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float specularIntensity;
		float specularPower;

		Texture *colorTexture;
		Texture *colorTexture2;
		Texture *colorTexture3;
		Texture *specularTexture;
		Texture *normalTexture;
		Texture *lightTexture;
		//Texture *heightTexture;

		Material(Texture *colorTexture, Texture *normalTexture = nullptr);
		Material(Texture *colorTexture, Texture *colorTexture2, Texture *colorTexture3);
	};
}
