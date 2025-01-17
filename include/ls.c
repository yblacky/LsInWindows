#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>

typedef struct FileEntry {
    char name[MAX_PATH];
    DWORD attributes;
    long size;
    time_t mtime;
} FileEntry;

void PrintFileDetails(const char* path, const char* fileName) {
    struct stat fileStat;
    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, fileName);

    if (stat(fullPath, &fileStat) == 0) {
        struct tm timeInfo;
        localtime_s(&timeInfo, &fileStat.st_mtime);

        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeInfo);

        printf("%10ld bytes  %s  ", fileStat.st_size, timeStr);
    } else {
        fprintf(stderr, "[Fehler beim Abrufen der Datei-Infos]  ");
    }
}

bool isDirectory(const char* path) {
    DWORD attributes = GetFileAttributes(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false; // Pfad existiert nicht
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

int CompareBySize(const void* a, const void* b) {
    FileEntry* fileA = (FileEntry*)a;
    FileEntry* fileB = (FileEntry*)b;
    return (fileB->size > fileA->size) - (fileB->size < fileA->size);
}

int CompareByTime(const void* a, const void* b) {
    FileEntry* fileA = (FileEntry*)a;
    FileEntry* fileB = (FileEntry*)b;
    return (fileB->mtime > fileA->mtime) - (fileB->mtime < fileA->mtime);
}

void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resConCol(void) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

void ReverseArray(FileEntry* files, int count) {
    for (int i = 0; i < count / 2; ++i) {
        FileEntry temp = files[i];
        files[i] = files[count - 1 - i];
        files[count - 1 - i] = temp;
    }
}

void PrintDirectoryContents(
    const char* path,
    bool showAll,
    bool longFormat,
    bool singleLine,
    bool recursive,
    bool showInodes,
    bool sortBySize,
    bool sortByTime,
    bool reverseOrder
) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Fehler beim Öffnen des Verzeichnisses: %s\n", path);
        return;
    }

    FileEntry files[1024];
    int fileCount = 0;

    do {
        const char* fileName = findFileData.cFileName;

        // Ignoriere "." und "..", wenn `-a` nicht gesetzt ist
        if (!showAll && (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)) {
            continue;
        }

        FileEntry entry;
        snprintf(entry.name, MAX_PATH, "%s", fileName);
        entry.attributes = findFileData.dwFileAttributes;

        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, fileName);

        struct stat fileStat;
        if (stat(fullPath, &fileStat) == 0) {
            entry.size = fileStat.st_size;
            entry.mtime = fileStat.st_mtime;
        } else {
            entry.size = 0;
            entry.mtime = 0;
        }

        files[fileCount++] = entry;

    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    // Sortierung
    if (sortBySize) {
        qsort(files, fileCount, sizeof(FileEntry), CompareBySize);
    } else if (sortByTime) {
        qsort(files, fileCount, sizeof(FileEntry), CompareByTime);
    }

    if (reverseOrder) {
        ReverseArray(files, fileCount);
    }

    // Dateien ausgeben
    for (int i = 0; i < fileCount; ++i) {
        if (showInodes) {
            printf("%10lu ", files[i].attributes); // Inode (fiktiv für Konsistenz)
        }
        if (longFormat) {
            PrintFileDetails(path, files[i].name);
        }

        SetConsoleColor(GetFileColor(path, files[i].name));
        printf("%s", files[i].name);
        resConCol(); // Farbe zurücksetzen

        if (singleLine) {
            printf("  ");
        } else {
            printf("\n");
        }
    }

    // Rekursive Verarbeitung
    if (recursive) {
        for (int i = 0; i < fileCount; ++i) {
            char fullPath[MAX_PATH];
            snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, files[i].name);
            if (isDirectory(fullPath) && strcmp(files[i].name, ".") != 0 && strcmp(files[i].name, "..") != 0) {
                printf("\nVerzeichnis: %s\n", fullPath);
                PrintDirectoryContents(fullPath, showAll, longFormat, singleLine, recursive, showInodes, sortBySize, sortByTime, reverseOrder);
            }
        }
    }
}

WORD GetFileColor(const char* path, const char* fileName) {
    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, fileName);

    DWORD attributes = GetFileAttributes(fullPath);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Standardfarbe
    }

    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        return FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Verzeichnis
    }

    const char* ext = strrchr(fileName, '.');
    if (ext != NULL) {
        if (strcmp(ext, ".exe") == 0 || strcmp(ext, ".dll") == 0) {
            return FOREGROUND_BLUE | FOREGROUND_INTENSITY; // Exe/DLL
        }
    }

    return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Standard
}
