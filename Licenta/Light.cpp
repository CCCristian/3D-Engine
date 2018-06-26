#include "Light.h"

namespace OpenGL
{
	extern int currentWindowWidth;
	extern int currentWindowHeight;

	Light::Light(bool isPointLight): shadowMap(Framebuffer::FramebufferType::DepthOnly, isPointLight)
	{

	}


	DirectionalLight::DirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor, glm::vec3 specularColor): Light(false)
	{
		DirectionalLight::direction = direction;
		DirectionalLight::diffuseIntensity = diffuseIntensity;
		DirectionalLight::diffuseColor = diffuseColor;
		DirectionalLight::specularColor = specularColor;
		computeShadowMatrix();
	}
	void DirectionalLight::computeShadowMatrix()
	{
		constexpr float border = 40.f;

		shadowViewMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 0, 1));
		shadowProjectionMatrix = glm::scale(glm::mat4(1), glm::vec3(1, 1, 0.05)) * glm::ortho(-border, border, -border, border);
		shadowMapMatrix = shadowProjectionMatrix * shadowViewMatrix;
	}
	void DirectionalLight::setDirection(glm::vec3 direction)
	{
		DirectionalLight::direction = direction;
		computeShadowMatrix();
	}
	void DirectionalLight::setPosition(glm::vec3 position)
	{
		DirectionalLight::position = position;
		computeShadowMatrix();
	}


	PointLight::PointLight(glm::vec3 position, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor): Light(true)
	{
		PointLight::position = position;
		PointLight::power = power;
		PointLight::quadraticFactor = quadraticFactor;
		PointLight::linearFactor = linearFactor;
		PointLight::constantFactor = constantFactor;
		PointLight::diffuseColor = diffuseColor;
		PointLight::specularColor = specularColor;
	}


	SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor): Light(false)
	{
		static const float PI = glm::pi<float>();

		SpotLight::position = position;
		SpotLight::direction = direction;
		SpotLight::innerAngle = innerAngleInDegrees * PI / 180;
		SpotLight::outerAngle = outerAngleInDegrees * PI / 180;
		SpotLight::innerAngleCosine = std::cos(innerAngleInDegrees * PI / 180);
		SpotLight::outerAngleCosine = std::cos(outerAngleInDegrees * PI / 180);
		SpotLight::power = power;
		SpotLight::quadraticFactor = quadraticFactor;
		SpotLight::linearFactor = linearFactor;
		SpotLight::constantFactor = constantFactor;
		SpotLight::diffuseColor = diffuseColor;
		SpotLight::specularColor = specularColor;

		computeShadowMatrix();
	}
	void SpotLight::computeShadowMatrix()
	{
		shadowViewMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
		shadowProjectionMatrix = glm::perspective(2 * outerAngle, (float)currentWindowWidth / currentWindowHeight, 0.01f, 100.f);
		shadowMapMatrix = shadowProjectionMatrix * shadowViewMatrix;
	}
	void SpotLight::setPosition(glm::vec3 position)
	{
		SpotLight::position = position;
		computeShadowMatrix();
	}
}
