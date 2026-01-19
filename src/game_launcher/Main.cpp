#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace fs = std::filesystem;

// ============================================
// ANSI Color Codes
// ============================================
namespace Color {
    const char* Reset     = "\033[0m";
    const char* Bold      = "\033[1m";
    const char* Dim       = "\033[2m";
    const char* Italic    = "\033[3m";
    const char* Underline = "\033[4m";
    
    // Foreground colors
    const char* Black   = "\033[30m";
    const char* Red     = "\033[31m";
    const char* Green   = "\033[32m";
    const char* Yellow  = "\033[33m";
    const char* Blue    = "\033[34m";
    const char* Magenta = "\033[35m";
    const char* Cyan    = "\033[36m";
    const char* White   = "\033[37m";
    
    // Bright foreground colors
    const char* BrightBlack   = "\033[90m";
    const char* BrightRed     = "\033[91m";
    const char* BrightGreen   = "\033[92m";
    const char* BrightYellow  = "\033[93m";
    const char* BrightBlue    = "\033[94m";
    const char* BrightMagenta = "\033[95m";
    const char* BrightCyan    = "\033[96m";
    const char* BrightWhite   = "\033[97m";
}

// ============================================
// Animation Utilities
// ============================================
void EnableANSI()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void ClearScreen()
{
    std::cout << "\033[2J\033[H";
}

void Sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void TypeWriter(const std::string& text, int delayMs = 15)
{
    for (char c : text)
    {
        std::cout << c << std::flush;
        Sleep(delayMs);
    }
}

void PrintAnimatedSpinner(const std::string& message, int durationMs)
{
    const char* frames[] = {"-", "\\", "|", "/"};
    const int frameCount = 4;
    int elapsed = 0;
    int frame = 0;
    
    while (elapsed < durationMs)
    {
        std::cout << "\r" << Color::Cyan << "[" << frames[frame] << "]" << Color::Reset << " " << message << std::flush;
        Sleep(100);
        elapsed += 100;
        frame = (frame + 1) % frameCount;
    }
    std::cout << "\r" << Color::Green << "[+]" << Color::Reset << " " << message << "          \n";
}

void PrintProgressBar(int percent, int width = 30)
{
    int filled = (width * percent) / 100;
    std::cout << "\r  " << Color::BrightBlue << "[";
    for (int i = 0; i < width; ++i)
    {
        if (i < filled)
            std::cout << Color::Cyan << "#";
        else
            std::cout << Color::BrightBlack << ".";
    }
    std::cout << Color::BrightBlue << "]" << Color::Reset << " " << percent << "%" << std::flush;
}

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
        std::string displayName = game.metadata.loaded && !game.metadata.displayName.empty() 
            ? game.metadata.displayName : game.name;
        
        std::cout << "\n  " << Color::Green << ">> " << Color::Reset;
        std::cout << Color::Bold << "Launching " << Color::BrightCyan << displayName << Color::Reset << "...\n";
        std::cout << Color::Dim << "  Waiting for game to close...\n\n" << Color::Reset;

        WaitForSingleObject(pi.hProcess, INFINITE);

        std::cout << "\n  " << Color::BrightBlue << "<<" << Color::Reset << " ";
        std::cout << Color::BrightCyan << displayName << Color::Reset << " has been closed.\n";

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
        std::cerr << Color::Red << "  [X] Failed to launch game. Error code: " << GetLastError() << Color::Reset << std::endl;
    }
#else
    // Linux/Unix: Use fork and exec
    pid_t pid = fork();

    if (pid == 0)
    {
        chdir(game.directory.c_str());
        execl(game.exePath.c_str(), game.exePath.c_str(), nullptr);

        std::cerr << Color::Red << "  [X] Failed to execute game" << Color::Reset << std::endl;
        exit(1);
    }
    else if (pid > 0)
    {
        std::string displayName = game.metadata.loaded && !game.metadata.displayName.empty() 
            ? game.metadata.displayName : game.name;
        
        std::cout << "\n  " << Color::Green << ">> " << Color::Reset;
        std::cout << Color::Bold << "Launching " << Color::BrightCyan << displayName << Color::Reset << "...\n";
        std::cout << Color::Dim << "  Waiting for game to close (PID: " << pid << ")...\n\n" << Color::Reset;

        int status;
        waitpid(pid, &status, 0);

        std::cout << "\n  " << Color::BrightBlue << "<<" << Color::Reset << " ";
        std::cout << Color::BrightCyan << displayName << Color::Reset << " has been closed.\n";
    }
    else
    {
        std::cerr << Color::Red << "  [X] Failed to fork process" << Color::Reset << std::endl;
    }
#endif
}

void DisplayMenu(const std::vector<GameInfo> &games)
{
    ClearScreen();
    
    // Animated header
    std::cout << Color::BrightCyan << Color::Bold;
    std::cout << R"(
    +-----------------------------------------------------------+
    |                                                           |
    |      ___   _  __ ___  ___   _   _                         |
    |     / _ \ | |/ /|_ _|/ _ \ | \ | |                        |
    |    / /_\ \|   <  | || | | ||  \| |                        |
    |    |  _  ||    \ | || |_| || |\  |                        |
    |    |_| |_||_|\_\|___|\___/ |_| \_|                        |
    |                                                           |
    |)" << Color::BrightMagenta << "              * E N G I N E *" << Color::BrightCyan << R"(                         |
    |                                                           |
    +-----------------------------------------------------------+
)" << Color::Reset;

    if (games.empty())
    {
        std::cout << "\n" << Color::Yellow << "  [!] " << Color::Reset;
        std::cout << "No games found in the current directory.\n";
        std::cout << Color::Dim << "    Make sure game executables are in their respective folders.\n" << Color::Reset;
        return;
    }

    std::cout << "\n" << Color::BrightWhite << Color::Bold << "  >> Available Games:" << Color::Reset << "\n\n";
    
    for (size_t i = 0; i < games.size(); ++i)
    {
        const auto& game = games[i];
        std::string displayName = game.metadata.loaded ? game.metadata.displayName : game.name;
        if (displayName.empty()) displayName = game.name;
        
        // Game number with color
        std::cout << "    " << Color::BrightBlue << Color::Bold << "[" << (i + 1) << "]" << Color::Reset << " ";
        
        // Game name
        std::cout << Color::BrightWhite << Color::Bold << displayName << Color::Reset;
        
        // Version badge
        if (game.metadata.loaded && !game.metadata.version.empty())
        {
            std::cout << " " << Color::BrightBlack << "v" << game.metadata.version << Color::Reset;
        }
        std::cout << "\n";
        
        // Tags with colored pills
        if (game.metadata.loaded && !game.metadata.tags.empty())
        {
            std::cout << "        ";
            const char* tagColors[] = {Color::Cyan, Color::Magenta, Color::Yellow, Color::Green, Color::Blue};
            for (size_t t = 0; t < game.metadata.tags.size() && t < 5; ++t)
            {
                std::cout << Color::Dim << "[" << tagColors[t % 5] << game.metadata.tags[t] << Color::Reset << Color::Dim << "]" << Color::Reset << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "    " << Color::Red << "[0]" << Color::Reset << " " << Color::Dim << "Exit" << Color::Reset << "\n";
    std::cout << "\n" << Color::BrightBlack << "  -----------------------------------------------------------\n" << Color::Reset;
}

void DisplayGameDetails(const GameInfo& game)
{
    std::string displayName = game.metadata.loaded ? game.metadata.displayName : game.name;
    if (displayName.empty()) displayName = game.name;
    
    std::cout << "\n";
    std::cout << Color::BrightCyan << "  +-----------------------------------------------------------+\n";
    std::cout << "  |" << Color::Reset;
    
    // Title centered
    std::cout << Color::Bold << Color::BrightWhite << " >> " << displayName;
    if (game.metadata.loaded && !game.metadata.version.empty())
        std::cout << Color::BrightBlack << " v" << game.metadata.version;
    std::cout << Color::Reset;
    
    // Padding to right border
    int titleLen = 4 + displayName.length() + (game.metadata.loaded && !game.metadata.version.empty() ? 2 + game.metadata.version.length() : 0);
    for (int i = titleLen; i < 58; ++i) std::cout << " ";
    std::cout << Color::BrightCyan << "|\n";
    
    if (game.metadata.loaded)
    {
        if (!game.metadata.author.empty())
        {
            std::cout << "  |" << Color::Reset << Color::Dim << "   By: " << game.metadata.author;
            int authorLen = 7 + game.metadata.author.length();
            for (int i = authorLen; i < 58; ++i) std::cout << " ";
            std::cout << Color::BrightCyan << "|\n";
        }
        
        std::cout << "  +-----------------------------------------------------------+\n" << Color::Reset;
        
        if (!game.metadata.description.empty())
        {
            // Word wrap description
            std::cout << Color::BrightCyan << "  |" << Color::Reset << "\n";
            std::string desc = game.metadata.description;
            size_t lineLen = 54;
            size_t pos = 0;
            while (pos < desc.length())
            {
                std::cout << Color::BrightCyan << "  |" << Color::Reset << "   ";
                std::cout << Color::White << desc.substr(pos, lineLen) << Color::Reset;
                pos += lineLen;
                std::cout << "\n";
            }
            std::cout << Color::BrightCyan << "  |" << Color::Reset << "\n";
        }
        
        if (!game.metadata.tags.empty())
        {
            std::cout << Color::BrightCyan << "  |" << Color::Reset << "   " << Color::Dim << "Tags: " << Color::Reset;
            const char* tagColors[] = {Color::Cyan, Color::Magenta, Color::Yellow, Color::Green, Color::Blue};
            for (size_t t = 0; t < game.metadata.tags.size(); ++t)
            {
                if (t > 0) std::cout << Color::Dim << ", " << Color::Reset;
                std::cout << tagColors[t % 5] << game.metadata.tags[t] << Color::Reset;
            }
            std::cout << "\n";
        }
    }
    
    std::cout << Color::BrightCyan << "  +-----------------------------------------------------------+\n" << Color::Reset;
}

int main()
{
    EnableANSI();
    
    ClearScreen();
    
    // Startup animation
    std::cout << "\n\n";
    std::cout << Color::BrightCyan << "  * " << Color::Reset;
    TypeWriter("Initializing Axion Engine Launcher...", 20);
    std::cout << "\n\n";
    
    // Loading animation
    for (int i = 0; i <= 100; i += 5)
    {
        PrintProgressBar(i);
        Sleep(30);
    }
    std::cout << "\n\n";
    
    PrintAnimatedSpinner("Scanning for games", 800);
    
    std::vector<GameInfo> games = FindGames();
    
    std::cout << Color::Green << "  [+] " << Color::Reset << "Found " << Color::BrightWhite << games.size() << Color::Reset << " game(s)\n";
    Sleep(500);

    while (true)
    {
        DisplayMenu(games);

        if (games.empty())
        {
            std::cout << "\n" << Color::Dim << "  Press Enter to exit..." << Color::Reset;
            std::cin.get();
            break;
        }

        std::cout << "\n  " << Color::BrightYellow << ">" << Color::Reset << " Select a game: " << Color::BrightWhite;

        int choice;
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << Color::Reset << "\n  " << Color::Red << "[X]" << Color::Reset << " Invalid input. Please enter a number.\n";
            Sleep(1000);
            continue;
        }
        std::cout << Color::Reset;

        std::cin.ignore(10000, '\n');

        if (choice == 0)
        {
            std::cout << "\n  " << Color::Dim << "Goodbye!" << Color::Reset << "\n\n";
            Sleep(500);
            break;
        }

        if (choice > 0 && choice <= static_cast<int>(games.size()))
        {
            DisplayGameDetails(games[choice - 1]);
            
            std::cout << "\n";
            PrintAnimatedSpinner("Preparing to launch", 600);
            
            LaunchGame(games[choice - 1]);
            
            std::cout << "\n  " << Color::Green << "[+]" << Color::Reset << " Game session ended.\n";
            std::cout << Color::Dim << "  Press Enter to continue..." << Color::Reset;
            std::cin.get();
        }
        else
        {
            std::cout << "\n  " << Color::Red << "[X]" << Color::Reset << " Invalid selection. Please try again.\n";
            Sleep(1000);
        }
    }

    return 0;
}
