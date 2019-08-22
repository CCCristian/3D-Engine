#pragma once

#include "GL.h"

namespace OpenGL
{
	class Shader
	{
	protected:
		static const int defaultBufferSize = 1024;
		GLuint program;
		GLuint addShader(const char *file, GLenum tip);
		GLuint createProgram(const char *vshaderSource, const char *fshaderSource);
		virtual void loadUniformLocations() = 0;
		virtual void initSamplers() = 0;

	public:
		enum ShaderType
		{
			MainShader,
			ShadowMapShader,
			SkyboxShader,
			WaterShader,
			DirectShader
		};
		Shader(const char *vshaderSource, const char *fshaderSource, ShaderType shaderType);
		~Shader()					{ glDeleteProgram(program); }
		GLuint getHandle() const	{ return program; }
		const ShaderType shaderType;
		const Shader& loadUniform(GLint location, const glm::vec3& vector) const;
		const Shader& loadUniform(GLint location, float x, float y, float z) const;
		const Shader& loadUniform(GLint location, const glm::vec4& vector) const;
		const Shader& loadUniform(GLint location, float x, float y, float z, float w) const;
		const Shader& loadUniform(GLint location, const glm::mat4& matrix) const;
		const Shader& loadUniform(GLint location, int integer) const;
		const Shader& loadUniform(GLint location, float number) const;
		const Shader& loadUniform(GLint location, bool boolean) const;


	private:
		struct SamplerValues
		{
			GLint colorSampler = 0;
			GLint colorSampler2 = 1;
			GLint colorSampler3 = 2;
			GLint blendSampler = 3;
			GLint specularSampler = 4;
			GLint normalSampler = 5;
			GLint dudvSampler = 6;
			GLint lightSampler = 7;
			GLint heightSampler = 8;
			GLint shadowSampler = 9;
			GLint skyboxSampler = 10;
			GLint reflectionSampler = 11;
			GLint refractionSampler = 12;
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
				GLint intensity;
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
			GLint clipPlane;

			GLint materialDiffuseColor;
			GLint materialSpecularColor;
			GLint materialSpecularIntensity;
			GLint materialSpecularPower;
			AmbientLight ambientLight;
			DirectionalLight directionalLight;
			PointLight pointLight;
			SpotLight spotLight;

			GLint textureCount;
			GLint hasNormalMap;
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

	class WaterShader: public Shader
	{
		struct UniformLocations
		{
			GLint world;
			GLint tiles;
			GLint transform;
			GLint movement;
			GLint cameraPosition;
			GLint directionalLight;
			GLint directionalColor;
			GLint reflectionSampler;
			GLint refractionSampler;
			GLint dudvSampler;
			GLint normalSampler;
		};

	protected:
		virtual void loadUniformLocations();
		virtual void initSamplers();
	public:
		static UniformLocations uniformLocations;
		WaterShader(const char *vshaderSource, const char *fshaderSource);
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
