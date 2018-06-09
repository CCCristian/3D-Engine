#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <fstream>
#include <string>

namespace OpenGL
{
	class Shader
	{
	protected:
		GLuint program;
		GLuint addShader(const char *file, GLenum tip);
		GLuint createProgram(const char *vshaderSource, const char *fshaderSource);
		virtual void loadUniformLocations() = 0;
		virtual void initSamplers() = 0;

	public:
		Shader(const char *vshaderSource, const char *fshaderSource);
		~Shader()			{ glDeleteProgram(program); }
		GLuint getHandle()	{ return program; }
		void use()			{ glUseProgram(program); }


	private:
		struct SamplerValues
		{
			GLint colorSampler = 0;
			GLint colorSampler2 = 1;
			GLint colorSampler3 = 2;
			GLint blendSampler = 3;
			GLint specularSampler = 4;
			GLint normalSampler = 5;
			GLint lightSampler = 6;
			GLint heightSampler = 7;
			GLint shadowSampler = 8;
			GLint skyboxSampler = 9;
		};
	public:
		static const SamplerValues samplerValues;
	};


	class SceneRenderingShader: public Shader
	{
		struct UniformLocations
		{
		private:
			struct AmbientLight
			{
				GLint color;
				GLint intensity;
			};
			struct DirectionalLight
			{
				GLint direction;
				GLint diffuseIntensity;
				GLint diffuseColor;
				GLint specularColor;
			};
			struct PointLight
			{
				GLint position;
				GLint power;
				GLint quadraticFactor;
				GLint linearFactor;
				GLint constantFactor;
				GLint diffuseColor;
				GLint specularColor;
			};
			struct SpotLight
			{
				GLint position;
				GLint direction;
				GLint innerAngleCosine;
				GLint outerAngleCosine;
				GLint power;
				GLint quadraticFactor;
				GLint linearFactor;
				GLint constantFactor;
				GLint diffuseColor;
				GLint specularColor;
			};

		public:
			GLint world;
			GLint lightSpaceMatrix;
			GLint transform;
			GLint cameraPosition;

			GLint materialDiffuseColor;
			GLint materialSpecularColor;
			GLint materialSpecularIntensity;
			GLint materialSpecularPower;
			AmbientLight ambientLight;
			DirectionalLight directionalLight;
			PointLight pointLight;
			SpotLight spotLight;

			GLint textureCount;
			GLint textureRepeatCount;
			GLint colorSampler;
			GLint colorSampler2;
			GLint colorSampler3;
			GLint blendSampler;
			GLint specularSampler;
			GLint normalSampler;
			GLint lightSampler;
			GLint heightSampler;
			GLint shadowSampler;
		};

	protected:
		virtual void loadUniformLocations();
		virtual void initSamplers();
	public:
		static UniformLocations uniformLocations;
		SceneRenderingShader(const char *vshaderSource, const char *fshaderSource);
	};

	class ShadowMapShader: public Shader
	{
		struct UniformLocations
		{
			GLint lightMatrix;
			GLint transform;
		};

	protected:
		virtual void loadUniformLocations();
		virtual void initSamplers();
	public:
		static UniformLocations uniformLocations;
		ShadowMapShader(const char *vshaderSource, const char *fshaderSource);
	};

	class SkyboxShader: public Shader
	{
		struct UniformLocations
		{
			GLint world;
			GLint skyboxSampler;
		};

	protected:
		virtual void loadUniformLocations();
		virtual void initSamplers();
	public:
		static UniformLocations uniformLocations;
		SkyboxShader(const char *vshaderSource, const char *fshaderSource);
	};

	class DirectShader: public Shader
	{
		struct UniformLocations
		{
			GLint transform;
			GLint colorSampler;
		};

		virtual void loadUniformLocations();
		virtual void initSamplers();
	public:
		static UniformLocations uniformLocations;
		DirectShader(const char* vshaderSource, const char* fshaderSource);;
	};
}
