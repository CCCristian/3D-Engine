#pragma once

#include "glm.hpp"
#include "Object.h"
#include "Framebuffer.h"

namespace OpenGL
{
	class Water: public Object
	{
		static constexpr float unitsPerTile = 10;

		static Texture* dudvMap;
		static Texture* normalMap;
		float height;
		glm::vec2 tiles;
		Framebuffer reflectionFramebuffer;
		Framebuffer refractionFramebuffer;
		
	public:
		static void initTextures();
		static const Texture* getDUDVMap()		{ return dudvMap; }
		static const Texture* getNormalMap()	{ return normalMap; }

		Water(float height, float northBorder, float southBorder, float eastBorder, float westBorder);
		float getHeight() const					{ return height; }
		const glm::vec2& getTiles() const		{ return tiles; }
		const Framebuffer& getReflectionFramebuffer() const	{ return reflectionFramebuffer; }
		const Framebuffer& getRefractionFramebuffer() const	{ return refractionFramebuffer; }
	};
}
