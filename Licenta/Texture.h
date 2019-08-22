#pragma once

#include "GL.h"

namespace OpenGL
{
	class Texture
	{
	public:
		enum TextureType
		{
			ColorTexture,
			DiffuseTexture,
			SpecularTexture,
			NormalMap,
			HeightMap,
			LightMap,
			ShadowMap
		};
		struct TextureData
		{
			unsigned char *data;
			int width, height;
			TextureData(std::string filePath);
			TextureData(const TextureData&) = delete;
			TextureData& operator =(const TextureData&) = delete;
			~TextureData();
		};
		
	private:
		static const std::string defaultTexturePath;
		static Texture *defaultTexture;
		static std::map<const std::string, Texture *> texturi;


		std::string filePath;
		GLuint tex;
		int width, height;
		int mipmapLevel;
		int pixelFormat;
		int imageFormat;
		GLenum dataType;

		Texture(const std::string filePath, int width, int height, int mipmapLevel, int pixelFormat, int imageFormat, GLenum dataType, const unsigned char *data);
		~Texture();
	public:
		static void init();
		static Texture* getDefaultTexture()		{ return defaultTexture; }
		static Texture* loadTexture(const std::string filePath, GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR, GLenum magFilter = GL_LINEAR, int mipmapLevel = 0);
		static void unloadTexture(const std::string filePath);
		static Texture* createShadowMap(int width, int height);
		GLuint getHandle() const	{ return tex; }
		int getMipmapLevel() const	{ return mipmapLevel; }
		int getWidth() const		{ return width; }
		int getHeight() const		{ return height; }
		void resize(int width, int height);


	public:
		class Builder
		{
			std::string filePath;
			int width, height;
			unsigned char* data;
			int mipmapLevel;
			int pixelFormat;
			int imageFormat;
			GLenum dataType;
			GLenum minfilter;
			GLenum magfilter;
			GLint wrapS;
			GLint wrapT;
			bool isCubemap;

		public:
			Builder();
			Builder& setFilePath(std::string filePath);
			Builder& setDimensions(int width, int height);
			Builder& setData(unsigned char* data);
			Builder& setMipmapLevel(int mipmapLevel);
			Builder& setImageFormat(TextureType type);
			Builder& setFilters(GLenum minfilter, GLenum magfilter);
			Builder& setWrapMode(GLint wrapS, GLint wrapT);
			Builder& setIsCubemap(bool isCubemap);
			Texture* build();
		};
	};
}
