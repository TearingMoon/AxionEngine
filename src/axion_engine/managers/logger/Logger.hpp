#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <SDL.h>
#include <format>
#include <vector>
#include <algorithm>

#include "axion_utilities/path/PathFinder.hpp"

namespace Axion
{

/**
 * @brief File-based logging system for the engine.
 * 
 * Logger provides timestamped log file output with automatic log rotation.
 * Supports multiple log levels: Info, Warn, Error, and custom messages.
 * 
 * @par Log Levels:
 * - **Info**: General information messages
 * - **Warn**: Warning conditions that may need attention  
 * - **Error**: Error conditions that affect operation
 */
class Logger
{
public:
    /**
     * @brief Constructs the logger and opens a new log file.
     * @param maxLogs Maximum number of log files to keep (older files are deleted)
     */
    explicit Logger(std::size_t maxLogs = 5);
    ~Logger();

    /**
     * @brief Logs an informational message.
     * @tparam Args Variadic template arguments for format
     * @param fmt Format string (std::format syntax)
     * @param args Format arguments
     */
    template <typename... Args>
    void Info(std::string_view fmt, Args&&... args);

    /**
     * @brief Logs a warning message.
     * @tparam Args Variadic template arguments for format
     * @param fmt Format string (std::format syntax)
     * @param args Format arguments
     */
    template <typename... Args>
    void Warn(std::string_view fmt, Args&&... args);

    /**
     * @brief Logs an error message.
     * @tparam Args Variadic template arguments for format
     * @param fmt Format string (std::format syntax)
     * @param args Format arguments
     */
    template <typename... Args>
    void Error(std::string_view fmt, Args&&... args);

    /**
     * @brief Logs a raw message without level prefix.
     * @tparam Args Variadic template arguments for format
     * @param fmt Format string (std::format syntax)
     * @param args Format arguments
     */
    template <typename... Args>
    void Log(std::string_view fmt, Args&&... args);

    /**
     * @brief Writes a visual separator line to the log.
     * @param title Text to display in the center of the separator
     * @param sepChar Character to use for the separator line
     * @param totalWidth Total width of the separator line
     */
    void Separator(std::string title = "", char sepChar = '-', int totalWidth = 50);

private:
    std::size_t maxLogs_;
    std::ofstream logFile_;
    std::string logPath_;

    void CleanupOldLogs();
};

template <typename... Args>
inline void Logger::Info(std::string_view fmt, Args&&... args)
{
    if (logFile_.is_open())
        logFile_ << "[INFO]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Warn(std::string_view fmt, Args&&... args)
{
    if (logFile_.is_open())
        logFile_ << "[WARN]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Error(std::string_view fmt, Args&&... args)
{
    if (logFile_.is_open())
        logFile_ << "[ERROR]: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

template <typename... Args>
inline void Logger::Log(std::string_view fmt, Args&&... args)
{
    if (logFile_.is_open())
        logFile_ << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

} // namespace Axion
