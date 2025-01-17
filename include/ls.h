#ifndef LS_H
#define LS_H

#include <windows.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    void SetConsoleColor(WORD color);
    void resConCol(void);
    void PrintFileDetails(const char* path, const char* fileName);
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
    );
    WORD GetFileColor(const char* path, const char* fileName); // Richtig deklarieren
    bool isDirectory(const char* path);

#ifdef __cplusplus
}
#endif

#endif // LS_H
