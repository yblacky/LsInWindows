#include "ls.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resConCol() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

int GetConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return 80;
    }
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
}

bool isDirectory(const char* path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISDIR(pathStat.st_mode);
}

void PrintFileDetails(const char* path, const std::string& fileName) {
    struct stat fileStat;
    std::string fullPath = std::string(path) + "/" + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0) {
        std::cout << std::setw(10) << fileStat.st_size << " bytes  ";
        std::cout << std::setw(20) << ctime(&fileStat.st_mtime) << "  ";
    } else {
        std::cerr << "[Fehler beim Abrufen der Datei-Infos]  ";
    }
}

void PrintDirectoryContents(const char* path, bool showAll, bool longFormat, bool singleLine, bool recursive) {
    DIR* dir;
    struct dirent* ent;
    std::vector<std::pair<std::string, WORD>> files;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = ent->d_name;
            WORD color;
            if (!showAll && (fileName == "." || fileName == "..")) continue;
            std::string fullPath = std::string(path) + "/" + fileName;
            if (isDirectory(fullPath.c_str())) {
                color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            } else if (fileName.substr(fileName.find_last_of(".") + 1) == "dll" || fileName.substr(fileName.find_last_of(".") + 1) == "exe") {
                color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            } else {
                color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            files.push_back(std::make_pair(fileName, color));
        }
        closedir(dir);
    } else {
        perror("Fehler beim Öffnen des Verzeichnisses");
        return;
    }

    std::sort(files.begin(), files.end());
    int consoleWidth = GetConsoleWidth();
    int currentLineLength = 0;
    int maxFileNameLength = 0;
    for (const auto& file : files) {
        int fileNameLength = file.first.length() + 1;
        if (fileNameLength > maxFileNameLength) {
            maxFileNameLength = fileNameLength;
        }
        if (currentLineLength + maxFileNameLength + 5 > consoleWidth && !singleLine) {
            std::cout << std::endl;
            currentLineLength = 0;
        }
        SetConsoleColor(file.second);
        if (longFormat) {
            PrintFileDetails(path, file.first);
        }
        std::cout << std::setw(maxFileNameLength) << std::setfill(' ') << std::left << file.first << "   ";
        currentLineLength += maxFileNameLength + 5;
        if (singleLine) {
            std::cout << "  ";
        }
    }
    std::cout << std::endl;
    SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    if (recursive) {
        for (const auto& file : files) {
            std::string fullPath = std::string(path) + "/" + file.first;
            if (file.first != "." && file.first != ".." && isDirectory(fullPath.c_str())) {
                std::cout << "\nVerzeichnis: " << fullPath << "\n";
                PrintDirectoryContents(fullPath.c_str(), showAll, longFormat, singleLine, recursive);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    bool showAll = false;
    bool longFormat = false;
    bool singleLine = false;
    bool recursive = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            showAll = true;
        } else if (strcmp(argv[i], "-l") == 0) {
            longFormat = true;
        } else if (strcmp(argv[i], "-m") == 0) {
            singleLine = true;
        } else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--recursive") == 0) {
            recursive = true;
        }
    }
    PrintDirectoryContents(".", showAll, longFormat, singleLine, recursive);
    resConCol();
    return 0;
}
