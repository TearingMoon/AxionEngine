#include "logger.hpp"

Logger::Logger(std::size_t maxLogs) : maxLogs_(maxLogs)
{

    std::time_t now = std::time(nullptr);
    std::tm tm{};

#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    std::string logName = oss.str();

    logPath_ = GetLoggerPath();
    std::filesystem::create_directories(logPath_);

    std::string logFilePath = logPath_ + logName + ".log";

    logFile_ = std::ofstream(logFilePath);
    if (logFile_.is_open())
    {
        Separator("Logger Started");
        logFile_ << "Log started at: " << std::put_time(&tm, "%c") << std::endl;
    }
    else
    {
        printf("Failed to open log file! at %s\n", logFilePath.c_str());
    }
}

Logger::~Logger()
{
    if (logFile_.is_open())
    {
        Separator("Logger Stopped");
        logFile_.close();
    }

    try
    {
        CleanupOldLogs();
    }
    catch (const std::exception &e)
    {
        printf("Failed to cleanup old logs: %s\n", e.what());
    }
}

void Logger::Separator(std::string title, char sepChar, int totalWidth)
{
    if (!logFile_.is_open())
        return;

    int titleLength = static_cast<int>(title.length());
    int sepLength = (totalWidth - titleLength - 2) / 2; // -2 for spaces around title

    logFile_ << std::string(sepLength, sepChar) << " " << title << " " << std::string(sepLength, sepChar);

    // If totalWidth is odd and titleLength is even (or vice versa), add one more sepChar
    if ((totalWidth - titleLength - 2) % 2 != 0)
    {
        logFile_ << sepChar;
    }

    logFile_ << std::endl;
}

std::string Logger::GetLoggerPath()
{
    namespace fs = std::filesystem;

    char *basePath = SDL_GetBasePath();

    fs::path root;

    if (basePath)
    {
        root = fs::path(basePath);
        SDL_free(basePath);
    }
    else
    {
        // Fallback: current working directory
        root = fs::current_path();
    }

    root /= "logs";

    std::string result = root.generic_string();
    if (!result.empty() && result.back() != '/')
        result.push_back('/');

    return result;
}

void Logger::CleanupOldLogs()
{
    if (maxLogs_ == 0)
        return;
    namespace fs = std::filesystem;

    std::vector<fs::directory_entry> logFiles;

    for (const auto &entry : fs::directory_iterator(logPath_))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() == ".log")
        {
            logFiles.push_back(entry);
        }
    }

    if (logFiles.size() <= maxLogs_)
        return;

    std::sort(logFiles.begin(), logFiles.end(),
              [](const fs::directory_entry &a, const fs::directory_entry &b)
              {
                  return fs::last_write_time(a) < fs::last_write_time(b);
              });

    const std::size_t toRemove = logFiles.size() - maxLogs_;

    for (std::size_t i = 0; i < toRemove; ++i)
    {
        try
        {
            fs::remove(logFiles[i].path());
        }
        catch (const std::exception &e)
        {
            printf("Failed to remove old log '%s': %s\n",
                   logFiles[i].path().string().c_str(), e.what());
        }
    }
}
