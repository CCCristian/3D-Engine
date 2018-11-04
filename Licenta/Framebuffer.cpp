#include "Framebuffer.h"
#include "Debug.h"

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

		GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (error != GL_FRAMEBUFFER_COMPLETE)
		{
			constexpr const char* error_GL_FRAMEBUFFER_UNDEFINED						= "GL_FRAMEBUFFER_UNDEFINED";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT			= "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT	= "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER			= "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER			= "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
			constexpr const char* error_GL_FRAMEBUFFER_UNSUPPORTED						= "GL_FRAMEBUFFER_UNSUPPORTED";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE			= "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
			constexpr const char* error_GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS			= "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

			const char* mesaj;
			switch (error)
			{
			case GL_FRAMEBUFFER_UNDEFINED:
				mesaj = error_GL_FRAMEBUFFER_UNDEFINED;						break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;	break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;		break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;		break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				mesaj = error_GL_FRAMEBUFFER_UNSUPPORTED;					break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE;		break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				mesaj = error_GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;		break;
			default:
				mesaj = "Alta eroare";
				break;
			}
			std::cerr << "Eroare: framebuffer incomplet - " << mesaj << ".\n";
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
