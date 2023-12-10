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

#ifdef _WIN32 // Windows
#define LOG(level, message) \
    do { \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
        SetConsoleTextAttribute(hConsole, level == INFO ? FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY : \
                            level == WARNING ? FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY : \
                            FOREGROUND_RED | FOREGROUND_INTENSITY); \
        std::cout << "[" << #level << "] " << message << std::endl; \
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); \
    } while (0)
#endif