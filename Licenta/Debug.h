#pragma once

#ifdef _DEBUG

static const char* error_GL_INVALID_ENUM					= "GL_INVALID_ENUM";
static const char* error_GL_INVALID_VALUE					= "GL_INVALID_VALUE";
static const char* error_GL_INVALID_OPERATION				= "GL_INVALID_OPERATION";
static const char* error_GL_INVALID_FRAMEBUFFER_OPERATION	= "GL_INVALID_FRAMEBUFFER_OPERATION";
static const char* error_GL_OUT_OF_MEMORY					= "GL_OUT_OF_MEMORY";
static const char* error_GL_STACK_UNDERFLOW					= "GL_STACK_UNDERFLOW";
static const char* error_GL_STACK_OVERFLOW					= "GL_STACK_OVERFLOW";
namespace
{
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

	template<typename Last> void __log(std::ostream& stream, const Last& last)
	{
		stream << last;
	}

	template<typename First, typename ... Rest> void __log(std::ostream& stream, const First& first, const Rest& ... rest)
	{
		stream << first;
		__log(stream, rest...);
	}

	template<typename ... T> void _log(const char* file, int line, bool error, T ... args)
	{
		std::ostream& stream = error ? std::cerr : std::cout;

		stream << "Log - " << file << '(' << line << "): ";
		__log(stream, args...);
		stream << "\n\n";
	}
}

#define checkErrors() {_checkErrors(__FILE__, __LINE__);}

#define Log(...)		_log(__FILE__, __LINE__, false, __VA_ARGS__)
#define LogError(...)	_log(__FILE__, __LINE__, true, __VA_ARGS__)

#else

#define checkErrors() ;
#define Log(...) ;
#define LogError(...) ;

#endif
