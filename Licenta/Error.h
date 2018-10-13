#pragma once

#ifdef _DEBUG

static const char* error_GL_INVALID_ENUM					= "GL_INVALID_ENUM";
static const char* error_GL_INVALID_VALUE					= "GL_INVALID_VALUE";
static const char* error_GL_INVALID_OPERATION				= "GL_INVALID_OPERATION";
static const char* error_GL_INVALID_FRAMEBUFFER_OPERATION	= "GL_INVALID_FRAMEBUFFER_OPERATION";
static const char* error_GL_OUT_OF_MEMORY					= "GL_OUT_OF_MEMORY";
static const char* error_GL_STACK_UNDERFLOW					= "GL_STACK_UNDERFLOW";
static const char* error_GL_STACK_OVERFLOW					= "GL_STACK_OVERFLOW";

void _checkErrors(const char* file, int line);

#define checkErrors() {_checkErrors(__FILE__, __LINE__);}

#else

#define checkErrors() ;

#endif
