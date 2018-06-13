#include "Water.h"
#include "Utils.h"
#include "glm//gtc/matrix_transform.hpp"

namespace OpenGL
{
	Texture* Water::dudvMap;
	Texture* Water::normalMap;

	void Water::initTextures()
	{
		dudvMap = Texture::loadTexture("Resurse/Water/WaterDUDV.png", GL_LINEAR, GL_LINEAR);
		normalMap = Texture::loadTexture("Resurse/Water/WaterNormal.png", GL_LINEAR, GL_LINEAR);
	}

	Water::Water(float height, float northBorder, float southBorder, float eastBorder, float westBorder): Object(Model::getQuadModel()),
			reflectionFramebuffer(Framebuffer::FramebufferType::Color),
			refractionFramebuffer(Framebuffer::FramebufferType::Color)
	{
		Water::height = height;
		transform = glm::translate(glm::mat4(1), glm::vec3(1, 1, 0));
		transform = glm::scale(glm::mat4(1), glm::vec3((eastBorder - westBorder) / 2, (northBorder - southBorder) / 2, 1)) * transform;
		transform = glm::translate(glm::mat4(1), glm::vec3(westBorder, southBorder, height)) * transform;
	}
	float Water::getHeight() const
	{
		return height;
	}
}
