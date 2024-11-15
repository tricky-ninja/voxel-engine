#include "Logger.h"
#include <ctime>
#include <iomanip>
#include <string>

#ifdef _WIN32
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif


static LogLevel s_currentLevel = LOG_INFO;
static std::ofstream s_fileStream;
static std::ostream* s_outputStream = &std::cout;
static std::stringstream s_nullStream;

static std::string logFile = "";

static const char* const s_logPrefix[] = {
"DEBUG: ",
"INFO: ",
"WARN! ",
"ERR!! ",
"ABORTED: ",
};

static std::string getCurrentTimestamp()
{
	std::time_t now = std::time(nullptr);
	std::tm* localTime = std::localtime(&now);

	std::ostringstream oss;
	oss << "["
		<< std::setfill('0') << std::setw(2) << localTime->tm_mday << "/"
		<< std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << "/"
		<< (localTime->tm_year + 1900) << "]:["
		<< std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << localTime->tm_sec << "]:";

	return oss.str();
}


void Logger::set_log_level(LogLevel level)
{
	s_currentLevel = level;
}

void Logger::set_output_file(const std::string& filepath)
{
	s_fileStream.open(filepath);
	if (s_fileStream.is_open())
	{
		s_outputStream = &s_fileStream;
		logFile = filepath;
	}
	else
	{
		s_outputStream = &std::cout;
	}
}

std::ostream& Logger::log(LogLevel level)
{
	if (level >= s_currentLevel)
	{
		std::string timestamp = getCurrentTimestamp();
		*s_outputStream << timestamp << s_logPrefix[level];
		return *s_outputStream;
	}

	return s_nullStream;
}

void perm_assert(bool condition)
{
#if PRODUCTION_BUILD == 0
	perm_assert_msg(condition, "Assert was triggered!");
#else
	perm_assert_msg(condition, "Something went wrong!");
#endif
}

void perm_assert_msg(bool condition, const char* msg)
{
	if (condition) return;
	Log_critical << "Assert_Failed: " << msg;

#ifdef _WIN32
#if PRODUCTION_BUILD == 0
	int code = MessageBoxA(0, msg, "Assertion failed", MB_ICONERROR | MB_ABORTRETRYIGNORE);
	if (code == IDABORT) exit(-1);
	if (code == IDRETRY)
	{
		__debugbreak();
	}
	return;
#else
	std::string message = "\nCheck " + logFile + " for more details\nPlease report this incident to the devs along with the log file";
	if (logFile == "") message = "\nUnfortunately no logs are avialable\nPlease report this incident to the devs";
	message = msg + message;
	MessageBoxA(0, message.c_str(), "Critical error", MB_ICONERROR | MB_OK);
#endif
#endif
	exit(-1);
}
