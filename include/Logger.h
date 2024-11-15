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
    static void set_log_level(LogLevel level);
    static void set_output_file(const std::string& filepath);
    static std::ostream& log(LogLevel level);
};

void perm_assert(bool condition);
void perm_assert_msg(bool condition, const char* msg);

#define Log_debug Logger::log(LOG_DEBUG)
#define Log_info Logger::log(LOG_INFO)
#define Log_warn Logger::log(LOG_WARN)
#define Log_error Logger::log(LOG_ERR)
#define Log_critical Logger::log(LOG_CRIT)
