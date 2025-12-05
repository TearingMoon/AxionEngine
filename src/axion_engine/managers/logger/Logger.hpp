#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <SDL.h>
#include <format>

class Logger
{

public:
    Logger(std::size_t maxLogs = 5);
    ~Logger();

    template <typename... Args>
    void Info(std::string_view fmt, Args &&...args);
    template <typename... Args>
    void Warn(std::string_view fmt, Args &&...args);
    template <typename... Args>
    void Error(std::string_view fmt, Args &&...args);
    template <typename... Args>
    void Log(std::string_view fmt, Args &&...args);
    void Separator(std::string title = "", char sepChar = '-', int totalWidth = 50);

private:
    std::size_t maxLogs_;
    std::ofstream logFile_;
    std::string logPath_;

    std::string GetLoggerPath();
    void CleanupOldLogs();
};

template <typename... Args>
inline void Logger::Info(std::string_view fmt, Args &&...args)
{
    if (logFile_.is_open())
        logFile_ << "[INFO]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Warn(std::string_view fmt, Args &&...args)
{
    if (logFile_.is_open())
        logFile_ << "[WARN]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Error(std::string_view fmt, Args &&...args)
{
    if (logFile_.is_open())
        logFile_ << "[ERROR]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Log(std::string_view fmt, Args &&...args)
{
    if (logFile_.is_open())
        logFile_ << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}
