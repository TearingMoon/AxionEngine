#include <iostream>
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

struct GameInfo {
    std::string name;
    fs::path exePath;
    fs::path directory;
};

std::vector<GameInfo> FindGames() {
    std::vector<GameInfo> games;
    
    fs::path launcherPath = fs::current_path();
    
    try {
        for (const auto& entry : fs::directory_iterator(launcherPath)) {
            if (entry.is_directory()) {
                std::string dirName = entry.path().filename().string();
                
                if (dirName == "logs" || dirName == "assets" || 
                    dirName == "assets_game2" || dirName.empty()) {
                    continue;
                }
                
                fs::path gameDir = entry.path();
                
#ifdef _WIN32
                fs::path exePath = gameDir / (dirName + ".exe");
#else
                fs::path exePath = gameDir / dirName;
#endif
                
                if (fs::exists(exePath)) {
                    GameInfo game;
                    game.name = dirName;
                    game.exePath = exePath;
                    game.directory = gameDir;
                    games.push_back(game);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error scanning directories: " << e.what() << std::endl;
    }
    
    return games;
}

void LaunchGame(const GameInfo& game) {
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
        NULL,                       // Application name
        cmdLine.data(),             // Command line
        NULL,                       // Process security attributes
        NULL,                       // Thread security attributes
        FALSE,                      // Inherit handles
        0,                          // Creation flags
        NULL,                       // Environment
        workingDir.c_str(),         // Current directory
        &si,                        // Startup info
        &pi                         // Process info
    )) {
        std::cout << "\nLaunching " << game.name << "...\n";
        std::cout << "Waiting for game to close...\n\n";
        
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        std::cout << "\n" << game.name << " has been closed.\n";
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow != NULL) {
            SetForegroundWindow(consoleWindow);
            ShowWindow(consoleWindow, SW_RESTORE);
        }
    } else {
        std::cerr << "Failed to launch game. Error code: " << GetLastError() << std::endl;
    }
#else
    // Linux/Unix: Use fork and exec
    pid_t pid = fork();
    
    if (pid == 0) {
        chdir(game.directory.c_str());
        execl(game.exePath.c_str(), game.exePath.c_str(), nullptr);
        
        std::cerr << "Failed to execute game" << std::endl;
        exit(1);
    } else if (pid > 0) {
        std::cout << "\nLaunching " << game.name << "...\n";
        std::cout << "Waiting for game to close (PID: " << pid << ")...\n\n";
        
        int status;
        waitpid(pid, &status, 0);
        
        std::cout << "\n" << game.name << " has been closed.\n";
    } else {
        std::cerr << "Failed to fork process" << std::endl;
    }
#endif
}

void DisplayMenu(const std::vector<GameInfo>& games) {
    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "       AXION ENGINE - GAME LAUNCHER        \n";
    std::cout << "============================================\n\n";
    
    if (games.empty()) {
        std::cout << "No games found in the current directory.\n";
        std::cout << "Make sure game executables are in their respective folders.\n";
        return;
    }
    
    std::cout << "Available Games:\n\n";
    for (size_t i = 0; i < games.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << games[i].name << "\n";
    }
    
    std::cout << "\n  [0] Exit\n";
    std::cout << "\n============================================\n";
}

int main() {
    std::vector<GameInfo> games = FindGames();
    
    while (true) {
        DisplayMenu(games);
        
        if (games.empty()) {
            std::cout << "\nPress Enter to exit...";
            std::cin.get();
            break;
        }
        
        std::cout << "\nSelect a game (enter number): ";
        
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }
        
        std::cin.ignore(10000, '\n');
        
        if (choice == 0) {
            std::cout << "\nExiting launcher...\n";
            break;
        }
        
        if (choice > 0 && choice <= static_cast<int>(games.size())) {
            LaunchGame(games[choice - 1]);
        } else {
            std::cout << "\nInvalid selection. Please try again.\n";
        }
    }
    
    return 0;
}
