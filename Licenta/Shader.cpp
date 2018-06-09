#include "Shader.h"
#include "Options.h"
#include "Error.h"

namespace OpenGL
{
	const Shader::SamplerValues Shader::samplerValues;
	SceneRenderingShader::UniformLocations SceneRenderingShader::uniformLocations;
	ShadowMapShader::UniformLocations ShadowMapShader::uniformLocations;
	SkyboxShader::UniformLocations SkyboxShader::uniformLocations;
	DirectShader::UniformLocations DirectShader::uniformLocations;

	Shader::Shader(const char *vshaderSource, const char *fshaderSource)
	{
#ifdef _DEBUG
		checkErrors();
#endif
		program = createProgram(vshaderSource, fshaderSource);
	}
	GLuint Shader::addShader(const char *file, GLenum tip)
	{
		std::string source, line;
		std::ifstream ii(file);
		while (line.length() == 0 || line[0] != '#')
			std::getline(ii, line);
		source += line + '\n';
#ifdef USE_INSTANCED_RENDERING
		source += "#define USE_INSTANCED_RENDERING\n";
#endif
		while (std::getline(ii, line))
			source += line + '\n';
		ii.close();
		const GLchar *strings = (const GLchar *)source.c_str();
		GLint l = source.length();
		GLuint shader = glCreateShader(tip);
		glShaderSource(shader, 1, &strings, &l);
		GLint succes;
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);
#ifdef _DEBUG
		checkErrors();
#endif
		if (succes)
		{
			int dim;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &dim);
			if (dim > 1)
			{
				GLchar *info = new GLchar[dim];
				glGetShaderInfoLog(shader, dim, 0, info);
				std::cout << "Compilare shader \"" << file << "\":\n" << info << "\n\n";
				delete info;
			}
		}
		else
		{
			int dim;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &dim);
			GLchar *info = new GLchar[dim];
			glGetShaderInfoLog(shader, dim, 0, info);
			std::cerr << "Eroare la compilare shader \"" << file << "\":\n" << info << "\n\n";
			std::cin.get();
			delete info;
			glDeleteShader(shader);
			exit(1);
		}
#ifdef _DEBUG
		checkErrors();
#endif
		return shader;
	}
	GLuint Shader::createProgram(const char *vshaderSource, const char *fshaderSource)
	{
		GLuint program = glCreateProgram();
		GLuint vshader = addShader(vshaderSource, GL_VERTEX_SHADER);
		GLuint fshader = addShader(fshaderSource, GL_FRAGMENT_SHADER);
		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		GLint succes;
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &succes);
#ifdef _DEBUG
		checkErrors();
#endif
		if (!succes)
		{
			int dim;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &dim);
			GLchar *info = new GLchar[dim];
			glGetProgramInfoLog(program, dim, 0, info);
			std::cerr << "Eroare la linkare program: " << (std::string(vshaderSource) + " + " + fshaderSource) << ":\n" << info << "\n\n";
			std::cin.get();
			delete info;
			exit(1);
		}
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &succes);
		if (!succes)
		{
			int dim;
			glGetShaderiv(program, GL_INFO_LOG_LENGTH, &dim);
			GLchar *info = new GLchar[dim];
			glGetProgramInfoLog(program, dim, 0, info);
			std::cerr << "Eroare la validare program : " << info << "\n\n";
			std::cin.get();
			delete info;
			exit(1);
		}
#ifdef _DEBUG
		checkErrors();
#endif
		glDetachShader(program, vshader);
		glDetachShader(program, fshader);
		return program;
	}
	

	SceneRenderingShader::SceneRenderingShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource)
	{
		SceneRenderingShader::loadUniformLocations();
		SceneRenderingShader::initSamplers();
#ifdef _DEBUG
		checkErrors();
#endif
	}
	void SceneRenderingShader::loadUniformLocations()
	{
		uniformLocations.world				= glGetUniformLocation(program, "world");
		uniformLocations.lightSpaceMatrix	= glGetUniformLocation(program, "lightSpaceMatrix");
		uniformLocations.transform			= glGetUniformLocation(program, "transform");
		uniformLocations.cameraPosition		= glGetUniformLocation(program, "cameraPosition");

		uniformLocations.materialDiffuseColor		= glGetUniformLocation(program, "materialDiffuseColor");
		uniformLocations.materialSpecularColor		= glGetUniformLocation(program, "materialSpecularColor");
		uniformLocations.materialSpecularIntensity	= glGetUniformLocation(program, "materialSpecularIntensity");
		uniformLocations.materialSpecularPower		= glGetUniformLocation(program, "materialSpecularPower");


		uniformLocations.ambientLight.color				= glGetUniformLocation(program, "ambientLight.color");
		uniformLocations.ambientLight.intensity			= glGetUniformLocation(program, "ambientLight.intensity");

		uniformLocations.directionalLight.direction			= glGetUniformLocation(program, "directionalLight.direction");
		uniformLocations.directionalLight.diffuseIntensity	= glGetUniformLocation(program, "directionalLight.diffuseIntensity");
		uniformLocations.directionalLight.diffuseColor		= glGetUniformLocation(program, "directionalLight.diffuseColor");
		uniformLocations.directionalLight.specularColor		= glGetUniformLocation(program, "directionalLight.specularColor");

		uniformLocations.pointLight.position		= glGetUniformLocation(program, "pointLight.position");
		uniformLocations.pointLight.power			= glGetUniformLocation(program, "pointLight.power");
		uniformLocations.pointLight.quadraticFactor	= glGetUniformLocation(program, "pointLight.quadraticFactor");
		uniformLocations.pointLight.linearFactor	= glGetUniformLocation(program, "pointLight.linearFactor");
		uniformLocations.pointLight.constantFactor	= glGetUniformLocation(program, "pointLight.constantFactor");
		uniformLocations.pointLight.diffuseColor	= glGetUniformLocation(program, "pointLight.diffuseColor");
		uniformLocations.pointLight.specularColor	= glGetUniformLocation(program, "pointLight.specularColor");

		uniformLocations.spotLight.position			= glGetUniformLocation(program, "spotLight.position");
		uniformLocations.spotLight.direction		= glGetUniformLocation(program, "spotLight.direction");
		uniformLocations.spotLight.innerAngleCosine	= glGetUniformLocation(program, "spotLight.innerAngleCosine");
		uniformLocations.spotLight.outerAngleCosine	= glGetUniformLocation(program, "spotLight.outerAngleCosine");
		uniformLocations.spotLight.power			= glGetUniformLocation(program, "spotLight.power");
		uniformLocations.spotLight.quadraticFactor	= glGetUniformLocation(program, "spotLight.quadraticFactor");
		uniformLocations.spotLight.linearFactor		= glGetUniformLocation(program, "spotLight.linearFactor");
		uniformLocations.spotLight.constantFactor	= glGetUniformLocation(program, "spotLight.constantFactor");
		uniformLocations.spotLight.diffuseColor		= glGetUniformLocation(program, "spotLight.diffuseColor");
		uniformLocations.spotLight.specularColor	= glGetUniformLocation(program, "spotLight.specularColor");


		uniformLocations.textureCount		= glGetUniformLocation(program, "textureCount");
		uniformLocations.textureRepeatCount = glGetUniformLocation(program, "textureRepeatCount");

		uniformLocations.colorSampler		= glGetUniformLocation(program, "colorSampler");
		uniformLocations.colorSampler2		= glGetUniformLocation(program, "colorSampler2");
		uniformLocations.colorSampler3		= glGetUniformLocation(program, "colorSampler3");
		uniformLocations.blendSampler		= glGetUniformLocation(program, "blendSampler");
		uniformLocations.specularSampler	= glGetUniformLocation(program, "specularSampler");
		uniformLocations.normalSampler		= glGetUniformLocation(program, "normalSampler");
		uniformLocations.lightSampler		= glGetUniformLocation(program, "lightSampler");
		uniformLocations.heightSampler		= glGetUniformLocation(program, "heightSampler");
		uniformLocations.shadowSampler		= glGetUniformLocation(program, "shadowSampler");
	}
	void SceneRenderingShader::initSamplers()
	{
		glUniform1i(uniformLocations.colorSampler,		samplerValues.colorSampler);
		glUniform1i(uniformLocations.colorSampler2,		samplerValues.colorSampler2);
		glUniform1i(uniformLocations.colorSampler3,		samplerValues.colorSampler3);
		glUniform1i(uniformLocations.blendSampler,		samplerValues.blendSampler);
		glUniform1i(uniformLocations.specularSampler,	samplerValues.specularSampler);
		glUniform1i(uniformLocations.normalSampler,		samplerValues.normalSampler);
		glUniform1i(uniformLocations.lightSampler,		samplerValues.lightSampler);
		glUniform1i(uniformLocations.heightSampler,		samplerValues.heightSampler);
		glUniform1i(uniformLocations.shadowSampler,		samplerValues.shadowSampler);
	}


	ShadowMapShader::ShadowMapShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource)
	{
		ShadowMapShader::loadUniformLocations();
		ShadowMapShader::initSamplers();
#ifdef _DEBUG
		checkErrors();
#endif
	}
	void ShadowMapShader::loadUniformLocations()
	{
		uniformLocations.transform = glGetUniformLocation(program, "transform");
		uniformLocations.lightMatrix = glGetUniformLocation(program, "lightMatrix");
	}
	void ShadowMapShader::initSamplers()
	{

	}


	SkyboxShader::SkyboxShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource)
	{
		SkyboxShader::loadUniformLocations();
		SkyboxShader::initSamplers();
#ifdef _DEBUG
		checkErrors();
#endif
	}
	void SkyboxShader::loadUniformLocations()
	{
		uniformLocations.world = glGetUniformLocation(program, "world");
		uniformLocations.skyboxSampler = glGetUniformLocation(program, "skyboxSampler");
	}
	void SkyboxShader::initSamplers()
	{
		glUniform1i(uniformLocations.skyboxSampler, samplerValues.skyboxSampler);
	}


	DirectShader::DirectShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource)
	{
		DirectShader::loadUniformLocations();
		DirectShader::initSamplers();
	}
	void DirectShader::loadUniformLocations()
	{
		uniformLocations.transform = glGetUniformLocation(program, "transform");
		uniformLocations.colorSampler = glGetUniformLocation(program, "colorSampler");
	}
	void DirectShader::initSamplers()
	{
		glUniform1i(uniformLocations.colorSampler, Shader::samplerValues.colorSampler);
	}
}
