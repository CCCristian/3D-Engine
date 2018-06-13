#include "Framebuffer.h"
#include "Error.h"

namespace OpenGL
{
	extern int currentWindowWidth;
	extern int currentWindowHeight;

	Framebuffer::Framebuffer(FramebufferType type, bool cubemap)
	{
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		switch (type)
		{
		case OpenGL::Framebuffer::Color:
			colorTexture = Texture::Builder().setDimensions(currentWindowWidth, currentWindowHeight).build();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->getHandle(), colorTexture->getMipmapLevel());
			depthTexture = Texture::createShadowMap(currentWindowWidth, currentWindowHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getHandle(), depthTexture->getMipmapLevel());
			checkErrors();
			break;
		case OpenGL::Framebuffer::DepthOnly:
			colorTexture = nullptr;
			depthTexture = Texture::createShadowMap(currentWindowWidth, currentWindowHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getHandle(), depthTexture->getMipmapLevel());
			glReadBuffer(GL_NONE);
			glDrawBuffer(GL_NONE);
			checkErrors();
			break;
		default:
			break;
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Eroare: framebuffer incomplet.\n";
			std::cin.get();
			exit(1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		checkErrors();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &fbo);
	}

	void Framebuffer::useAndClear() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkErrors();
	}

	void Framebuffer::bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void Framebuffer::resize(int width, int height)
	{
		colorTexture->resize(width, height);
		depthTexture->resize(width, height);
		checkErrors();
	}

	//void Framebuffer::resizeAllFramebuffersToCurrentWindow()
	//{
	//	for (Framebuffer* fb : framebuffers)
	//		fb->resize(currentWindowWidth, currentWindowHeight);
	//}
}
