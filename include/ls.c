#include "ls.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>
#include <stdbool.h> /
#include <time.h>

void PrintFileDetails(const char* path, const char* fileName) {
    struct stat fileStat;
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, fileName);
    if (stat(fullPath, &fileStat) == 0) {
        printf("%10ld bytes  %s", fileStat.st_size, ctime(&fileStat.st_mtime));
    } else {
        fprintf(stderr, "[Fehler beim Abrufen der Datei-Infos]  ");
    }
}

bool isDirectory(const char* path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISDIR(pathStat.st_mode);
}

void PrintDirectoryContents(const char* path, int showAll, int longFormat, int singleLine, int recursive) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            const char* fileName = ent->d_name;
            if (!showAll && (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)) continue;
            if (longFormat) {
                PrintFileDetails(path, fileName);
            }
            printf("%s   ", fileName);
            if (singleLine) {
                printf("  ");
            } else {
                printf("\n");
            }
        }
        closedir(dir);
    } else {
        perror("Fehler beim Öffnen des Verzeichnisses");
    }
    if (recursive) {
        dir = opendir(path);
        if (dir != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (isDirectory(ent->d_name) && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    char newPath[1024];
                    snprintf(newPath, sizeof(newPath), "%s/%s", path, ent->d_name);
                    printf("\nVerzeichnis: %s\n", newPath);
                    PrintDirectoryContents(newPath, showAll, longFormat, singleLine, recursive);
                }
            }
            closedir(dir);
        }
    }
}
