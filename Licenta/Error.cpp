#include "Error.h"

#ifdef _DEBUG

#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"

void checkErrors()
{
	//GLenum error = glGetError();
	//const char* textEroare = 0;
	//if (error | GL_INVALID_ENUM)
	//	textEroare = error_GL_INVALID_ENUM;
	//else if (error | GL_INVALID_VALUE)
	//	textEroare = error_GL_INVALID_VALUE;
	//else if (error | GL_INVALID_OPERATION)
	//	textEroare = error_GL_INVALID_OPERATION;
	//else if (error | GL_INVALID_FRAMEBUFFER_OPERATION)
	//	textEroare = error_GL_INVALID_FRAMEBUFFER_OPERATION;
	//else if (error | GL_OUT_OF_MEMORY)
	//	textEroare = error_GL_OUT_OF_MEMORY;
	//else if (error | GL_STACK_UNDERFLOW)
	//	textEroare = error_GL_STACK_UNDERFLOW;
	//else if (error | GL_STACK_OVERFLOW)
	//	textEroare = error_GL_STACK_OVERFLOW;

	//std::cerr << "Eroare OpenGL: " << textEroare << ".\n";
	//std::cin.get();
}
#endif
