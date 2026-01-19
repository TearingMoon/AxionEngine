#pragma once
#include <string>
#include <filesystem>
#include <SDL.h>

namespace Axion
{

/**
 * @brief Get the directory containing the executable.
 * @return Absolute path ending with '/'.
 */
inline std::string GetExecutablePath()
{
    namespace fs = std::filesystem;

    char* basePath = SDL_GetBasePath();

    fs::path root;

    if (basePath)
    {
        root = fs::path(basePath);
        SDL_free(basePath);
    }
    else
    {
        root = fs::current_path();
    }

    std::string result = root.generic_string();
    if (!result.empty() && result.back() != '/')
        result.push_back('/');

    return result;
}

/**
 * @brief Resolve a relative path from the executable directory.
 * @param relativePath Path relative to executable.
 * @return Absolute path ending with '/'.
 */
inline std::string GetPath(std::string relativePath)
{
    namespace fs = std::filesystem;

    fs::path root = fs::path(GetExecutablePath());
    root /= relativePath;

    std::string result = root.generic_string();
    if (!result.empty() && result.back() != '/')
        result.push_back('/');

    return result;
}

} // namespace Axion