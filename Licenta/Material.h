#pragma once

#include "Texture.h"


namespace OpenGL
{
	class Material
	{
		/* Default parameter values. */
		static const glm::vec3 defaultDiffuseColor;
		static const glm::vec3 defaultSpecularColor;
		static constexpr float defaultSpecularIntensity = 0.4f;
		static constexpr float defaultSpecularPower = 20;

		/** These are material keys used by Assimp. */
		struct MaterialKeysStruct
		{
			std::string diffuseColor = "$clr.diffuse";
			std::string specularColor = "$clr.specular";
			std::string specularIntensity = "$mat.shinpercent";
			std::string specularPower = "$mat.shininess";
		};


		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float specularIntensity;
		float specularPower;

		const Texture *colorTexture;
		const Texture *colorTexture2;
		const Texture *colorTexture3;
		const Texture *normalTexture;
		const Texture *specularTexture;
		//const Texture *lightTexture;
		//const Texture *heightTexture;

	public:
		/** Material property names used by Assimp. */
		static const MaterialKeysStruct MaterialKeys;

		const glm::vec3& getDiffuseColor()			{ return diffuseColor; }
		const glm::vec3& getSpecularColor()			{ return specularColor; }
		float getSpecularIntensity()				{ return specularIntensity; }
		float getSpecularPower()					{ return specularPower; }
		bool hasNormalMap()							{ return normalTexture != nullptr; }
		const Texture* getColorTexture() const		{ return colorTexture; }
		const Texture* getColorTexture2() const		{ return colorTexture2; }
		const Texture* getColorTexture3() const		{ return colorTexture3; }
		const Texture* getNormalTexture() const		{ return normalTexture; }
		const Texture* getSpecularTexture() const	{ return specularTexture; }
		//Texture* getLightTexture() const	{ return lightTexture; }
		//Texture* getHeightTexture() const	{ return heightTexture; }



		class Builder
		{
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
			float specularIntensity;
			float specularPower;

			const Texture *colorTexture;
			const Texture *colorTexture2;
			const Texture *colorTexture3;
			const Texture *normalTexture;
			const Texture *specularTexture;
			//const Texture *lightTexture;
			//const Texture *heightTexture;

		public:
			Builder(const Texture* colorTexture = nullptr);
			Material* build();

			Builder& setDiffuseColor(const glm::vec3& diffuseColor);
			Builder& setSpecularColor(const glm::vec3& specularColor);
			Builder& setSpecularIntensity(float specularIntensity);
			Builder& setSpecularPower(float specularPower);
			Builder& setColorTexture(const Texture* colorTexture);
			Builder& setColorTexture2(const Texture* colorTexture2);
			Builder& setColorTexture3(const Texture* colorTexture3);
			Builder& setNormalTexture(const Texture* specularTexture);
			Builder& setSpecularTexture(const Texture* normalTexture);
		};
	};
}
