#ifndef LS_H
#define LS_H

#include <windows.h>
#include <stdbool.h> // Für den Typ bool

void SetConsoleColor(WORD color);
void resConCol();
int GetConsoleWidth();
void PrintFileDetails(const char* path, const char* fileName);
void PrintDirectoryContents(const char* path, int showAll, int longFormat, int singleLine, int recursive);
bool isDirectory(const char* path);

#endif // LS_H
