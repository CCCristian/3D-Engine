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
		glm::mat4 shadowViewMatrix;
		glm::mat4 shadowProjectionMatrix;
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

		glm::vec3 position;		// For shadow mapping
	public:
		DirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor, glm::vec3 specularColor);
		~DirectionalLight() {}
		void computeShadowMatrix();
		glm::vec3 getDirection() const			{ return direction; }
		glm::vec3 getDiffuseColor() const		{ return diffuseColor; }
		glm::vec3 getSpecularColor() const		{ return specularColor; }
		glm::mat4* getShadowMapMatrix()			{ return &shadowMapMatrix; }
		glm::mat4* getShadowViewMatrix()		{ return &shadowViewMatrix; }
		glm::mat4* getShadowProjectionMatrix()	{ return &shadowProjectionMatrix; }
		void setDirection(glm::vec3 direction);
		void setPosition(glm::vec3 position);
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
		void computeShadowMatrix();
		glm::vec3 getPosition() const			{ return position; };
		void setPosition(glm::vec3 position);
		glm::mat4* getShadowMapMatrix()			{ return &shadowMapMatrix; }
		glm::mat4* getShadowViewMatrix()		{ return &shadowViewMatrix; }
		glm::mat4* getShadowProjectionMatrix()	{ return &shadowProjectionMatrix; }
	};
}
