#include "Error.h"

#ifdef _DEBUG

#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"

void _checkErrors(const char* file, int line)
{
	GLenum error = glGetError();
	const char* textEroare = 0;
	switch (error)
	{
	case GL_NO_ERROR:						return;
	case GL_INVALID_ENUM:					textEroare = error_GL_INVALID_ENUM;						break;
	case GL_INVALID_VALUE:					textEroare = error_GL_INVALID_VALUE;					break;
	case GL_INVALID_OPERATION:				textEroare = error_GL_INVALID_OPERATION;				break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:	textEroare = error_GL_INVALID_FRAMEBUFFER_OPERATION;	break;
	case GL_OUT_OF_MEMORY:					textEroare = error_GL_OUT_OF_MEMORY;					break;
	case GL_STACK_UNDERFLOW:				textEroare = error_GL_STACK_UNDERFLOW;					break;
	case GL_STACK_OVERFLOW:					textEroare = error_GL_STACK_OVERFLOW;					break;
	default:
		return;
	}

	std::cerr << "Eroare OpenGL: " << textEroare << ".\n";
	std::cerr << "Fisierul: " << file << ".\n";
	std::cerr << "Linia: " << line << ".\n\n";
}

#endif
