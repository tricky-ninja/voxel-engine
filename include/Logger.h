#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

enum LogLevel
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERR,
    LOG_CRIT
};

struct Logger
{
    static void setLogLevel(LogLevel level);
    static void setOutputFile(const std::string& filepath);
    static std::ostream& log(LogLevel level);
};

void permAssert(bool condition);
void permAssert_msg(bool condition, const char* msg);

#define Log_debug Logger::log(LOG_DEBUG)
#define Log_info Logger::log(LOG_INFO)
#define Log_warn Logger::log(LOG_WARN)
#define Log_error Logger::log(LOG_ERR)
#define Log_critical Logger::log(LOG_CRIT)
