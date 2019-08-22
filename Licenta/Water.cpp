#include "pch.h"
#include "Water.h"
#include "Utils.h"

namespace OpenGL
{
	Texture* Water::dudvMap;
	Texture* Water::normalMap;

	void Water::initTextures()
	{
		dudvMap = Texture::loadTexture("Resurse/Water/WaterDUDV.png");
		normalMap = Texture::loadTexture("Resurse/Water/WaterNormal.png");
	}

	Water::Water(float height, float northBorder, float southBorder, float eastBorder, float westBorder): Object(Model::getQuadModel()),
			reflectionFramebuffer(Framebuffer::FramebufferType::Color),
			refractionFramebuffer(Framebuffer::FramebufferType::Color)
	{
		Water::height = height;
		Water::tiles = glm::vec2(eastBorder - westBorder, northBorder - southBorder) / unitsPerTile;
		transform = glm::translate(glm::mat4(1), glm::vec3(1, 1, 0));
		transform = glm::scale(glm::mat4(1), glm::vec3((eastBorder - westBorder) / 2, (northBorder - southBorder) / 2, 1)) * transform;
		transform = glm::translate(glm::mat4(1), glm::vec3(westBorder, southBorder, height)) * transform;
	}
}
