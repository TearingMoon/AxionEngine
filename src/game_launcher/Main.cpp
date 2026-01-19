#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace fs = std::filesystem;

/**
 * @brief Metadata loaded from game.json
 */
struct GameMetadata
{
    std::string displayName;
    std::string description;
    std::string version;
    std::string author;
    std::vector<std::string> tags;
    bool loaded = false;
};

struct GameInfo
{
    std::string name;
    fs::path exePath;
    fs::path directory;
    GameMetadata metadata;
};

/**
 * @brief Simple JSON string value parser (no external dependencies)
 */
std::string ParseJsonString(const std::string& json, const std::string& key)
{
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos)
        return "";
    
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos)
        return "";
    
    size_t startQuote = json.find('"', colonPos);
    if (startQuote == std::string::npos)
        return "";
    
    size_t endQuote = json.find('"', startQuote + 1);
    if (endQuote == std::string::npos)
        return "";
    
    return json.substr(startQuote + 1, endQuote - startQuote - 1);
}

/**
 * @brief Parse JSON array of strings
 */
std::vector<std::string> ParseJsonStringArray(const std::string& json, const std::string& key)
{
    std::vector<std::string> result;
    
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos)
        return result;
    
    size_t bracketStart = json.find('[', keyPos);
    if (bracketStart == std::string::npos)
        return result;
    
    size_t bracketEnd = json.find(']', bracketStart);
    if (bracketEnd == std::string::npos)
        return result;
    
    std::string arrayContent = json.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
    
    size_t pos = 0;
    while ((pos = arrayContent.find('"', pos)) != std::string::npos)
    {
        size_t endPos = arrayContent.find('"', pos + 1);
        if (endPos == std::string::npos)
            break;
        
        result.push_back(arrayContent.substr(pos + 1, endPos - pos - 1));
        pos = endPos + 1;
    }
    
    return result;
}

/**
 * @brief Load game metadata from game.json
 */
GameMetadata LoadGameMetadata(const fs::path& gameDir)
{
    GameMetadata metadata;
    fs::path jsonPath = gameDir / "game.json";
    
    if (!fs::exists(jsonPath))
        return metadata;
    
    try
    {
        std::ifstream file(jsonPath);
        if (!file.is_open())
            return metadata;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        
        metadata.displayName = ParseJsonString(json, "name");
        metadata.description = ParseJsonString(json, "description");
        metadata.version = ParseJsonString(json, "version");
        metadata.author = ParseJsonString(json, "author");
        metadata.tags = ParseJsonStringArray(json, "tags");
        metadata.loaded = true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Warning: Could not parse game.json: " << e.what() << std::endl;
    }
    
    return metadata;
}

std::vector<GameInfo> FindGames()
{
    std::vector<GameInfo> games;

    fs::path launcherPath = fs::current_path();

    try
    {
        for (const auto &entry : fs::directory_iterator(launcherPath))
        {
            if (entry.is_directory())
            {
                std::string dirName = entry.path().filename().string();

                // if (dirName == "logs" || dirName == "assets" ||
                //     dirName == "assets_game2" || dirName.empty()) {
                //     continue;
                // }

                fs::path gameDir = entry.path();

#ifdef _WIN32
                fs::path exePath = gameDir / (dirName + ".exe");
#else
                fs::path exePath = gameDir / dirName;
#endif

                if (fs::exists(exePath))
                {
                    GameInfo game;
                    game.name = dirName;
                    game.exePath = exePath;
                    game.directory = gameDir;
                    game.metadata = LoadGameMetadata(gameDir);
                    games.push_back(game);
                }
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "Error scanning directories: " << e.what() << std::endl;
    }

    return games;
}

void LaunchGame(const GameInfo &game)
{
#ifdef _WIN32
    // Windows: Use CreateProcess
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string exePathStr = game.exePath.string();
    std::string workingDir = game.directory.string();

    std::vector<char> cmdLine(exePathStr.begin(), exePathStr.end());
    cmdLine.push_back('\0');

    if (CreateProcessA(
            NULL,               // Application name
            cmdLine.data(),     // Command line
            NULL,               // Process security attributes
            NULL,               // Thread security attributes
            FALSE,              // Inherit handles
            0,                  // Creation flags
            NULL,               // Environment
            workingDir.c_str(), // Current directory
            &si,                // Startup info
            &pi                 // Process info
            ))
    {
        std::cout << "\nLaunching " << game.name << "...\n";
        std::cout << "Waiting for game to close...\n\n";

        WaitForSingleObject(pi.hProcess, INFINITE);

        std::cout << "\n"
                  << game.name << " has been closed.\n";

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow != NULL)
        {
            SetForegroundWindow(consoleWindow);
            ShowWindow(consoleWindow, SW_RESTORE);
        }
    }
    else
    {
        std::cerr << "Failed to launch game. Error code: " << GetLastError() << std::endl;
    }
#else
    // Linux/Unix: Use fork and exec
    pid_t pid = fork();

    if (pid == 0)
    {
        chdir(game.directory.c_str());
        execl(game.exePath.c_str(), game.exePath.c_str(), nullptr);

        std::cerr << "Failed to execute game" << std::endl;
        exit(1);
    }
    else if (pid > 0)
    {
        std::cout << "\nLaunching " << game.name << "...\n";
        std::cout << "Waiting for game to close (PID: " << pid << ")...\n\n";

        int status;
        waitpid(pid, &status, 0);

        std::cout << "\n"
                  << game.name << " has been closed.\n";
    }
    else
    {
        std::cerr << "Failed to fork process" << std::endl;
    }
#endif
}

void DisplayMenu(const std::vector<GameInfo> &games)
{
    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "       AXION ENGINE - GAME LAUNCHER        \n";
    std::cout << "============================================\n\n";

    if (games.empty())
    {
        std::cout << "No games found in the current directory.\n";
        std::cout << "Make sure game executables are in their respective folders.\n";
        return;
    }

    std::cout << "Available Games:\n\n";
    for (size_t i = 0; i < games.size(); ++i)
    {
        const auto& game = games[i];
        std::string displayName = game.metadata.loaded ? game.metadata.displayName : game.name;
        if (displayName.empty()) displayName = game.name;
        
        std::cout << "  [" << (i + 1) << "] " << displayName;
        
        if (game.metadata.loaded && !game.metadata.version.empty())
        {
            std::cout << " v" << game.metadata.version;
        }
        std::cout << "\n";
        
        if (game.metadata.loaded && !game.metadata.tags.empty())
        {
            std::cout << "      Tags: ";
            for (size_t t = 0; t < game.metadata.tags.size(); ++t)
            {
                if (t > 0) std::cout << ", ";
                std::cout << game.metadata.tags[t];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "  [0] Exit\n";
    std::cout << "\n============================================\n";
}

void DisplayGameDetails(const GameInfo& game)
{
    std::cout << "\n--------------------------------------------\n";
    
    std::string displayName = game.metadata.loaded ? game.metadata.displayName : game.name;
    if (displayName.empty()) displayName = game.name;
    
    std::cout << "  " << displayName;
    if (game.metadata.loaded && !game.metadata.version.empty())
        std::cout << " v" << game.metadata.version;
    std::cout << "\n";
    
    if (game.metadata.loaded)
    {
        if (!game.metadata.author.empty())
            std::cout << "  By: " << game.metadata.author << "\n";
        
        std::cout << "--------------------------------------------\n";
        
        if (!game.metadata.description.empty())
        {
            std::cout << "\n  " << game.metadata.description << "\n";
        }
        
        if (!game.metadata.tags.empty())
        {
            std::cout << "\n  Tags: ";
            for (size_t t = 0; t < game.metadata.tags.size(); ++t)
            {
                if (t > 0) std::cout << ", ";
                std::cout << game.metadata.tags[t];
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "--------------------------------------------\n";
}

int main()
{
    std::vector<GameInfo> games = FindGames();

    while (true)
    {
        DisplayMenu(games);

        if (games.empty())
        {
            std::cout << "\nPress Enter to exit...";
            std::cin.get();
            break;
        }

        std::cout << "\nSelect a game (enter number): ";

        int choice;
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore(10000, '\n');

        if (choice == 0)
        {
            std::cout << "\nExiting launcher...\n";
            break;
        }

        if (choice > 0 && choice <= static_cast<int>(games.size()))
        {
            DisplayGameDetails(games[choice - 1]);
            LaunchGame(games[choice - 1]);
        }
        else
        {
            std::cout << "\nInvalid selection. Please try again.\n";
        }
    }

    return 0;
}
