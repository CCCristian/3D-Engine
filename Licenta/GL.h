#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "Debug.h"

#ifdef _DEBUG

#define _glDrawElementsInstanced(...) GLEW_GET_FUN(__glewDrawElementsInstanced)(__VA_ARGS__)
#define _glDrawArraysInstanced(...) GLEW_GET_FUN(__glewDrawElementsInstanced)(__VA_ARGS__)
//#undef glDrawElements
#undef glDrawElementsInstanced
//#undef glDrawArrays
#undef glDrawArraysInstanced

static bool validateShader()
{
	static const int defaultBufferSize = 1024;
	static GLchar info[defaultBufferSize];

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	GLint succes;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &succes);
	if (!succes)
	{
		//glGetShaderiv(program, GL_INFO_LOG_LENGTH, &dim);
		glGetProgramInfoLog(program, defaultBufferSize, 0, info);
		LogError("Eroare la validare program : ", info);
	}
	return succes;
}

#define glDrawElements(...)\
{\
	validateShader();\
	glDrawElements(__VA_ARGS__);\
}\

#define glDrawElementsInstanced(...)\
{\
	validateShader();\
	_glDrawElementsInstanced(__VA_ARGS__);\
}

#define glDrawArrays(...)\
{\
	validateShader();\
	glDrawArrays(__VA_ARGS__);\
}\

#define glDrawArraysInstanced(...)\
{\
	validateShader();\
	_glDrawArraysInstanced(__VA_ARGS__);\
}

#endif
