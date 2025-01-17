#include "ls.h"
#include <iostream>
#include <iomanip>
#include <sys/stat.h>

// Definiere S_ISDIR für Windows
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif


int GetConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return 80;
    }
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
}

int main(int argc, char* argv[]) {
    // Flag-Variablen
    bool showAll = false;
    bool longFormat = false;
    bool singleLine = false;
    bool recursive = false;
    bool showInodes = false;    // -i
    bool sortBySize = false;    // -S
    bool sortByTime = false;    // -t
    bool reverseOrder = false;  // -r

    // Verzeichnispfad prüfen
    const char* targetPath = ".";
    if (argc > 1 && argv[argc - 1][0] != '-') {
        targetPath = argv[argc - 1];
        struct stat pathStat;
        if (stat(targetPath, &pathStat) != 0 || !S_ISDIR(pathStat.st_mode)) {
            std::cerr << "Ungültiger Pfad: " << targetPath << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Flags verarbeiten
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                switch (argv[i][j]) {
                case 'a': showAll = true; break;
                case 'l': longFormat = true; break;
                case 'm': singleLine = true; break;
                case 'R': recursive = true; break;
                case 'i': showInodes = true; break;
                case 'S': sortBySize = true; break;
                case 't': sortByTime = true; break;
                case 'r': reverseOrder = true; break;
                default:
                    std::cerr << "Unbekanntes Flag: -" << argv[i][j] << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        std::cout << "Verwendung: ls [Optionen] [Pfad]\n"
                  << "Optionen:\n"
                  << "  -a  Zeigt alle Dateien an, einschließlich versteckter Dateien\n"
                  << "  -l  Zeigt Dateien im langen Format\n"
                  << "  -m  Gibt Dateien in einer einzigen Zeile aus\n"
                  << "  -R  Zeigt Verzeichnisse rekursiv an\n"
                  << "  -i  Zeigt die Inode-Nummer jeder Datei\n"
                  << "  -S  Sortiert nach Dateigröße\n"
                  << "  -t  Sortiert nach Änderungszeit\n"
                  << "  -r  Kehrt die Sortierreihenfolge um\n";
        return 0;
    }

    PrintDirectoryContents(targetPath, showAll, longFormat, singleLine, recursive, showInodes, sortBySize, sortByTime, reverseOrder);
    resConCol();
    return 0;
}
