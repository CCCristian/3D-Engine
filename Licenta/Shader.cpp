#include "Shader.h"
#include "Options.h"
#include "Debug.h"

namespace OpenGL
{
	const Shader::SamplerValues Shader::samplerValues;
	SceneRenderingShader::UniformLocations SceneRenderingShader::uniformLocations;
	ShadowMapShader::UniformLocations ShadowMapShader::uniformLocations;
	SkyboxShader::UniformLocations SkyboxShader::uniformLocations;
	WaterShader::UniformLocations WaterShader::uniformLocations;
	DirectShader::UniformLocations DirectShader::uniformLocations;

	Shader::Shader(const char *vshaderSource, const char *fshaderSource, ShaderType shaderType): shaderType(shaderType)
	{
		program = createProgram(vshaderSource, fshaderSource);
		glUseProgram(program);
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
		if (succes)
		{
			int dim = defaultBufferSize;
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
			int dim = defaultBufferSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &dim);
			GLchar *info = new GLchar[dim];
			glGetShaderInfoLog(shader, dim, 0, info);
			std::cerr << "Eroare la compilare shader \"" << file << "\":\n" << info << "\n\n";
			std::cin.get();
			delete info;
			glDeleteShader(shader);
			exit(1);
		}
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
		if (!succes)
		{
			int dim = defaultBufferSize;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &dim);
			GLchar *info = new GLchar[dim];
			glGetProgramInfoLog(program, dim, 0, info);
			std::cerr << "Eroare la linkare program: " << (std::string(vshaderSource) + " + " + fshaderSource) << ":\n" << info << "\n\n";
			std::cin.get();
			delete info;
			exit(1);
		}
		glDetachShader(program, vshader);
		glDetachShader(program, fshader);
		return program;
	}
	const Shader& Shader::loadUniform(GLint location, const glm::vec3& vector) const
	{
		glUniform3f(location, vector.x, vector.y, vector.z);
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, float x, float y, float z) const
	{
		glUniform3f(location, x, y, z);
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, const glm::vec4& vector) const
	{
		glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, float x, float y, float z, float w) const
	{
		glUniform4f(location, x, y, z, w);
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, const glm::mat4& matrix) const
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(&matrix));
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, int integer) const
	{
		glUniform1i(location, integer);
		checkErrors();
		return *this;
	}
	const Shader& Shader::loadUniform(GLint location, float number) const
	{
		glUniform1f(location, number);
		checkErrors();
		return *this;
	}

	const Shader& Shader::loadUniform(GLint location, bool boolean) const
	{
		glUniform1i(location, boolean);
		checkErrors();
		return *this;
	}
	
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	SceneRenderingShader::SceneRenderingShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource, ShaderType::MainShader)
	{
		SceneRenderingShader::loadUniformLocations();
		SceneRenderingShader::initSamplers();
		checkErrors();
	}
	void SceneRenderingShader::loadUniformLocations()
	{
		uniformLocations.world				= glGetUniformLocation(program, "world");
		uniformLocations.lightSpaceMatrix	= glGetUniformLocation(program, "lightSpaceMatrix");
		uniformLocations.transform			= glGetUniformLocation(program, "transform");
		uniformLocations.cameraPosition		= glGetUniformLocation(program, "cameraPosition");
		uniformLocations.clipPlane			= glGetUniformLocation(program, "clipPlane");

		uniformLocations.materialDiffuseColor		= glGetUniformLocation(program, "materialDiffuseColor");
		uniformLocations.materialSpecularColor		= glGetUniformLocation(program, "materialSpecularColor");
		uniformLocations.materialSpecularIntensity	= glGetUniformLocation(program, "materialSpecularIntensity");
		uniformLocations.materialSpecularPower		= glGetUniformLocation(program, "materialSpecularPower");


		uniformLocations.ambientLight.color				= glGetUniformLocation(program, "ambientLight.color");
		uniformLocations.ambientLight.intensity			= glGetUniformLocation(program, "ambientLight.intensity");

		uniformLocations.directionalLight.direction			= glGetUniformLocation(program, "directionalLight.direction");
		uniformLocations.directionalLight.intensity			= glGetUniformLocation(program, "directionalLight.intensity");
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
		uniformLocations.hasNormalMap		= glGetUniformLocation(program, "hasNormalMap");
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

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	ShadowMapShader::ShadowMapShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource, ShaderType::ShadowMapShader)
	{
		ShadowMapShader::loadUniformLocations();
		ShadowMapShader::initSamplers();
		checkErrors();
	}
	void ShadowMapShader::loadUniformLocations()
	{
		uniformLocations.transform = glGetUniformLocation(program, "transform");
		uniformLocations.lightMatrix = glGetUniformLocation(program, "lightMatrix");
	}
	void ShadowMapShader::initSamplers()
	{

	}

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	SkyboxShader::SkyboxShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource, ShaderType::SkyboxShader)
	{
		SkyboxShader::loadUniformLocations();
		SkyboxShader::initSamplers();
		checkErrors();
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

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	WaterShader::WaterShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource, ShaderType::WaterShader)
	{
		WaterShader::loadUniformLocations();
		WaterShader::initSamplers();
	}
	void WaterShader::loadUniformLocations()
	{
		uniformLocations.world				= glGetUniformLocation(program, "world");
		uniformLocations.tiles				= glGetUniformLocation(program, "tiles");
		uniformLocations.transform			= glGetUniformLocation(program, "transform");
		uniformLocations.movement			= glGetUniformLocation(program, "movement");
		uniformLocations.cameraPosition		= glGetUniformLocation(program, "cameraPosition");
		uniformLocations.directionalLight	= glGetUniformLocation(program, "directionalLight");
		uniformLocations.directionalColor	= glGetUniformLocation(program, "directionalColor");
		uniformLocations.reflectionSampler	= glGetUniformLocation(program, "reflectionSampler");
		uniformLocations.refractionSampler	= glGetUniformLocation(program, "refractionSampler");
		uniformLocations.dudvSampler		= glGetUniformLocation(program, "dudvSampler");
		uniformLocations.normalSampler		= glGetUniformLocation(program, "normalSampler");
	}
	void WaterShader::initSamplers()
	{
		glUniform1i(uniformLocations.reflectionSampler,	Shader::samplerValues.reflectionSampler);
		glUniform1i(uniformLocations.refractionSampler,	Shader::samplerValues.refractionSampler);
		glUniform1i(uniformLocations.dudvSampler,		Shader::samplerValues.dudvSampler);
		glUniform1i(uniformLocations.normalSampler,		Shader::samplerValues.normalSampler);
	}

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	DirectShader::DirectShader(const char* vshaderSource, const char* fshaderSource): Shader(vshaderSource, fshaderSource, ShaderType::DirectShader)
	{
		DirectShader::loadUniformLocations();
		DirectShader::initSamplers();
	}
	void DirectShader::loadUniformLocations()
	{
		uniformLocations.transform		= glGetUniformLocation(program, "transform");
		uniformLocations.colorSampler	= glGetUniformLocation(program, "colorSampler");
	}
	void DirectShader::initSamplers()
	{
		glUniform1i(uniformLocations.colorSampler, Shader::samplerValues.colorSampler);
	}
}
