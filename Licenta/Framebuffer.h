#pragma once

#include "GL/glew.h"
#include "GL\freeglut.h"
#include "Texture.h"
#include <iostream>
#include <set>

namespace OpenGL
{
	class Framebuffer
	{
		GLuint fbo;
		Texture *colorTexture, *depthTexture;
	public:
		enum FramebufferType
		{
			Color, DepthOnly
		};
		Framebuffer(FramebufferType type, bool cubemap = false);
		Framebuffer(const Framebuffer&) = delete;
		~Framebuffer();
		void useAndClear() const;
		void bind() const;
		GLuint getHandle() const			{ return fbo; }
		Texture* getColorTexture() const	{ return colorTexture; }
		Texture* getDepthTexture() const	{ return depthTexture; }

		/** Resize the framebuffer's texture to the given dimensions. */
		void resize(int width, int height);

		///** Take the current window's dimensions and resize all framebuffers' textures accordingly. */
		//static void resizeAllFramebuffersToCurrentWindow();
	};
}
