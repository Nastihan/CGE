#pragma once
#include <iostream>

namespace CGE
{
    // Define log levels
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    // Logging macro
#define LOG_CONSOLE(level, message) \
    do \
    { \
        if (level >= LogLevel::Info) \
        { \
            std::wcout << "[" << #level << "] " << __FILE__ << ":" << __LINE__ << " - " << message << std::endl; \
        } \
    } while (false)
}