#include "openGLError.h"
#include "Logger.h"
#include <cassert>

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	LogLevel errorLevel = LOG_WARN;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:		 errorLevel = LOG_CRIT; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       errorLevel = LOG_ERR; break;
	case GL_DEBUG_SEVERITY_LOW:          errorLevel = LOG_WARN; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: errorLevel = LOG_INFO; break;
	}

	Logger::log(errorLevel) << "---------------" << std::endl;
	Logger::log(errorLevel) << "[OpenGL ERROR](" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             Logger::log(errorLevel) << "Source: API\n"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   Logger::log(errorLevel) << "Source: Window System\n"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: Logger::log(errorLevel) << "Source: Shader Compiler\n"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     Logger::log(errorLevel) << "Source: Third Party\n"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     Logger::log(errorLevel) << "Source: Application\n"; break;
	case GL_DEBUG_SOURCE_OTHER:           Logger::log(errorLevel) << "Source: Other\n"; break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               Logger::log(errorLevel) << "Type: Error\n"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Logger::log(errorLevel) << "Type: Deprecated Behaviour\n"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  Logger::log(errorLevel) << "Type: Undefined Behaviour\n"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         Logger::log(errorLevel) << "Type: Portability\n"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         Logger::log(errorLevel) << "Type: Performance\n"; break;
	case GL_DEBUG_TYPE_MARKER:              Logger::log(errorLevel) << "Type: Marker\n"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          Logger::log(errorLevel) << "Type: Push Group\n"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           Logger::log(errorLevel) << "Type: Pop Group\n"; break;
	case GL_DEBUG_TYPE_OTHER:               Logger::log(errorLevel) << "Type: Other\n"; break;
	};

	if (errorLevel == LOG_CRIT) permAssert_msg(false, std::string(std::string("OpenGL ERROR: ") + message).c_str());


}