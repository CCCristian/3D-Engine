#pragma once

#include "Model.h"

namespace OpenGL
{
	class Terrain: public Model
	{
		static const int heightMapChannelCount = 3;
		int textureCount;
		float textureRepeatCount;
		float getHeightMapFactor(unsigned char* data, int width, int x, int y);
	public:
		Terrain(std::string name, std::string heightMapFile, Texture* texture1, Texture* texture2, Texture* texture3, float maxHeight, Material* material);
		virtual ~Terrain();
		/** Upload all necessary non-instance uniforms to the shader, bind the VAO, bind the appropriate textures, etc. */
		virtual void prepareShader(int meshObjectIndex) const;


		class Builder
		{
			std::string name;
			std::string heightMapFile;
			Texture* texture1;
			Texture* texture2;
			Texture* texture3;
			float maxHeight;
			float textureRepeatCount;
			
			int addedTexturesCount;
		public:
			Builder();
			Builder& setName(std::string name);
			Builder& setHeightMapFile(std::string heightMapFile);
			Builder& addTexture(std::string texture);
			Builder& setMaxHeight(float maxHeight);
			Builder& setTextureRepeatCount(float textureRepeatCount);
			Terrain* build();
		};
	};
}
