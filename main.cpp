#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>


#include "DirData.h"
#include "Unpack.h"
#include "Pack.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

void Help()
{
    printf("\n\n");
    printf("    programName -v\n");
    printf("    programName -o pack   -d folder_to_pack   -f output\n");
    printf("    programName -o unpack -d folder_to_unpack -f packdata\n");
    printf("\n");
}

void ResizeConsole(int Width, int Height)
{
    HANDLE pConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (pConsoleHandle == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    SMALL_RECT srWindowRect = { 0 };
    COORD coordScreen = { 0 };

    GetConsoleScreenBufferInfo(pConsoleHandle, &csbi);
    coordScreen = GetLargestConsoleWindowSize(pConsoleHandle);

    srWindowRect.Right  = (SHORT)(MIN(Width, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT)(MIN(Height, coordScreen.Y) - 1);

    coordScreen.X = Width;
    coordScreen.Y = Height;

    if( (DWORD)csbi.dwSize.X * csbi.dwSize.Y > (DWORD) Width * Height)
    {
        SetConsoleWindowInfo(pConsoleHandle, TRUE, &srWindowRect);
        SetConsoleScreenBufferSize(pConsoleHandle, coordScreen);
    }

    if( (DWORD)csbi.dwSize.X * csbi.dwSize.Y < (DWORD) Width * Height )
    {
        SetConsoleScreenBufferSize(pConsoleHandle, coordScreen);
        SetConsoleWindowInfo(pConsoleHandle, TRUE, &srWindowRect);
    }
}

bool ArgsParser(int argc, char *argv[], char *l_pFileO, char *l_pFolder, char *l_pOption)
{
    if(argc < 7) return false;

    unsigned int _o = 0;
    unsigned int _d = 0;
    unsigned int _f = 0;

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-o"))
        {
            strcpy_s(l_pOption, 32, argv[++i]);
            _o++;
        }
        else if(!strcmp(argv[i], "-d"))
        {
            strcpy_s(l_pFolder, MAX_PATH, argv[++i]);
            _d++;
        }
        else if(!strcmp(argv[i], "-f"))
        {
            strcpy_s(l_pFileO, MAX_PATH, argv[++i]);
            _f++;
        }
    }

    return _f == 1 && _d == 1 && _o == 1;
}

int main(int argc, char *argv[])
{
    ResizeConsole(200, 30);

    if((argc == 2 && !strcmp(argv[1], "-v")) || argc == 1) 
    {
        Help();
        system("pause");
        return EXIT_SUCCESS;
    }

    char l_pFileO[MAX_PATH] = { 0 };
    char l_pFolder[MAX_PATH] = { 0 };

    char l_pOption[32] = { 0 };
    if(!ArgsParser(argc, argv, l_pFileO, l_pFolder, l_pOption))
    {
        Help();
        system("pause");
        return EXIT_SUCCESS;
    }

    FILE *l_pFile = NULL;
    S_PDIRDATA t = NULL;

    if(!strcmp(l_pOption, "pack"))
    {
        t = ListDir(l_pFolder);

        fopen_s(&l_pFile, l_pFileO, "wb");
        PackFolder(t, l_pFile);
    }
    else if(!strcmp(l_pOption, "unpack"))
    {
        fopen_s(&l_pFile, l_pFileO, "rb");
        t = UnpackFolder(l_pFile, l_pFolder);
    }
    else
    {
        Help();
        system("pause");
        return EXIT_SUCCESS;
    }

    fclose(l_pFile);
    DeleteDirData(t);

    system("pause");
    return EXIT_SUCCESS;
}