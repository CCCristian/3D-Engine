#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Error.h"

namespace OpenGL
{
	extern int currentWindowWidth;
	extern int currentWindowHeight;

	std::map<const std::string, Texture *> Texture::texturi;
	const std::string Texture::defaultTexturePath = "Resurse/Texturi/Default.bmp";
	Texture *Texture::defaultTexture;

	Texture::Texture(const std::string filePath, int width, int height, int mipmapLevel, int pixelFormat, int imageFormat, GLenum dataType, const unsigned char *data)
	{
		Texture::filePath = filePath;
		Texture::width = width;
		Texture::height = height;
		Texture::mipmapLevel = mipmapLevel;
		Texture::imageFormat = imageFormat;
		Texture::dataType = dataType;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		if (imageFormat == GL_DEPTH_COMPONENT)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, pixelFormat, width, height, 0, imageFormat, dataType, data);
		if (data)		// glGenerateMipmap must be called even if mipmapLevel == 0
		{
			glGenerateMipmap(GL_TEXTURE_2D);
#ifdef _DEBUG
			checkErrors();
#endif
		}
	}
	Texture::~Texture()
	{
		glDeleteTextures(1, &tex);
	}
	void Texture::resize(int width, int height)
	{
		Texture::width = width;
		Texture::height = height;
		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, pixelFormat, Texture::width, Texture::height, 0, imageFormat, dataType, 0);
#ifdef _DEBUG
		checkErrors();
#endif
	}
	Texture* Texture::loadTexture(std::string filePath, int mipmapLevel)
	{
		if (filePath == "" || filePath[filePath.length() - 1] == '/' || filePath[filePath.length() - 1] == '\\')
			return defaultTexture;
		auto it = texturi.find(filePath);
		if (it == texturi.end())
		{
			int w, h;
			unsigned char *imagine = stbi_load(filePath.c_str(), &w, &h, 0, STBI_rgb);
			if (imagine == 0)
			{
				std::cerr << "Eroare la incarcarea texturii: \"" << filePath << "\"\n";
				//std::cerr << "STB: " << "" << "\n\n";
				return texturi.find(defaultTexturePath)->second;
			}
			else
			{
				Texture *tex = Builder().setFilePath(filePath).setDimensions(w, h).setData(imagine).setMipmapLevel(mipmapLevel).setFilter(GL_LINEAR_MIPMAP_LINEAR).build();
				texturi.insert(std::make_pair(filePath, tex));
				return tex;
			}
			stbi_image_free(imagine);
		}
		else
			return it->second;
	}
	void Texture::unloadTexture(const std::string filePath)
	{
		auto it = texturi.find(filePath);
		if (it != texturi.end())
		{
			delete it->second;
			texturi.erase(it);
		}
	}
	Texture* Texture::createShadowMap(int width, int height)
	{
		return Builder().setDimensions(width, height).setImageFormat(TextureType::ShadowMap).build();
	}
	void Texture::init()
	{
		// Load the default texture
		int w, h;
		unsigned char *imagine = stbi_load(defaultTexturePath.c_str(), &w, &h, 0, STBI_rgb);
		if (imagine == 0)
		{
			std::cerr << "Eroare la incarcarea texturii default: \"" << defaultTexturePath << "\"\n";
			std::cin.get();
			//SOIL_free_image_data(imagine);
			exit(1);
		}
		else
		{
			Texture *tex = Builder().setFilePath(defaultTexturePath).setDimensions(w, h).setData(imagine).build();
			texturi.insert(std::make_pair(defaultTexturePath, tex));
			defaultTexture = tex;
		}
	}


	Texture::TextureData::TextureData(std::string filePath)
	{
		if (filePath == "" || filePath[filePath.length() - 1] == '/' || filePath[filePath.length() - 1] == '\\')
		{
			std::cerr << "Eroare la incarcarea texturii: \"" << filePath << "\" - niciun fisier specificat.\n";
			data = nullptr;
		}
		else
		{
			data = stbi_load(filePath.c_str(), &width, &height, 0, STBI_rgb);
			if (data == 0)
			{
				std::cerr << "Eroare la incarcarea texturii: \"" << filePath << "\"\n";
				//std::cerr << "SOIL: " << SOIL_last_result() << "\n\n";
			}
		}
	}
	Texture::TextureData::~TextureData()
	{
		if (data != nullptr)
			stbi_image_free(data);
	}


	Texture::Builder::Builder()
	{
		filePath = "";
		width = currentWindowWidth;
		height = currentWindowHeight;
		data = 0;
		mipmapLevel = 0;
		pixelFormat = GL_RGB;
		imageFormat = GL_RGB;
		dataType = GL_UNSIGNED_BYTE;
		filter = GL_NEAREST;
		isCubemap = false;
	}
	Texture::Builder& Texture::Builder::setFilePath(std::string filePath)
	{
		Builder::filePath = filePath;
		return *this;
	}
	Texture::Builder& Texture::Builder::setDimensions(int width, int height)
	{
		Builder::width = width;
		Builder::height = height;
		return *this;
	}
	Texture::Builder& Texture::Builder::setData(unsigned char* data)
	{
		Builder::data = data;
		return *this;
	}
	Texture::Builder& Texture::Builder::setMipmapLevel(int mipmapLevel)
	{
		Builder::mipmapLevel = mipmapLevel;
		return *this;
	}
	Texture::Builder& Texture::Builder::setImageFormat(TextureType type)
	{
		switch (type)
		{
		case OpenGL::Texture::ColorTexture:
			pixelFormat = GL_RGB;
			imageFormat = GL_RGB;
			dataType = GL_UNSIGNED_BYTE;
			break;
		case OpenGL::Texture::ShadowMap:
			pixelFormat = GL_DEPTH_COMPONENT;
			imageFormat = GL_DEPTH_COMPONENT;
			dataType = GL_FLOAT;
			break;
		//case OpenGL::Texture::SpecularTexture:
		//	imageFormat = GL_RGB;
		//	break;
		//case OpenGL::Texture::NormalMap:
		//	imageFormat = GL_RGB;
		//	break;
		//case OpenGL::Texture::HeightMap:
		//	imageFormat = GL_RGB;
		//	break;
		//case OpenGL::Texture::LightMap:
		//	imageFormat = GL_RGB;
		//	break;
		default:
			break;
		}
		return *this;
	}
	Texture::Builder& Texture::Builder::setFilter(GLenum filter)
	{
		Builder::filter = filter;
		return *this;
	}
	Texture::Builder& Texture::Builder::setIsCubemap(bool isCubemap)
	{
		Builder::isCubemap = isCubemap;
		return *this;
	}
	Texture* Texture::Builder::build()
	{
		Texture* tex = new Texture(filePath, width, height, mipmapLevel, pixelFormat, imageFormat, dataType, data);
		glBindTexture(GL_TEXTURE_2D, tex->tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		return tex;
	}
	void OpenGL::Texture::Builder::deleteTexture(Texture* texture)
	{
		delete texture;
	}
}
