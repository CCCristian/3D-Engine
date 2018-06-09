#pragma once

#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Framebuffer.h"
#include "Object.h"

namespace OpenGL
{
	class Light
	{
	protected:
		Framebuffer shadowMap;
		glm::mat4 shadowMapMatrix;
	public:
		Light(bool isPointLight);
		virtual ~Light() {}
		Framebuffer* getShadowFramebuffer()		{ return &shadowMap; }
	};


	class DirectionalLight: public Light
	{
		glm::vec3 direction;
		float diffuseIntensity;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
	public:
		DirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor, glm::vec3 specularColor);
		~DirectionalLight() {}
		void setDirection(glm::vec3 direction);
		glm::mat4* getShadowMapMatrix()			{ return &shadowMapMatrix; }
	};
	class PointLight: public Light
	{
		glm::vec3 position;
		float power;
		float quadraticFactor;
		float linearFactor;
		float constantFactor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
	public:
		PointLight(glm::vec3 position, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor);
		~PointLight() {}
		glm::vec3 getPosition() const			{ return position; };
	};
	class SpotLight: public Light
	{
		glm::vec3 position;
		glm::vec3 direction;
		float innerAngle;
		float outerAngle;
		float innerAngleCosine;
		float outerAngleCosine;
		float power;
		float quadraticFactor;
		float linearFactor;
		float constantFactor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
	public:
		SpotLight(glm::vec3 position, glm::vec3 direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor);
		~SpotLight() {}
		glm::vec3 getPosition() const			{ return position; };
		void setPosition(glm::vec3 position);
		glm::mat4* getShadowMapMatrix()			{ return &shadowMapMatrix; }
	};
}