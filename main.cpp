#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>


#include "DirData.h"
#include "Unpack.h"
#include "Pack.h"

void Help()
{
    printf("\n\n");
    printf("programName -v\n");
    printf("programName -o pack   -d folder_to_pack   -f output\n");
    printf("programName -o unpack -d folder_to_unpack -f packdata\n");
    printf("\n");
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
    HANDLE l_pConsoleH = GetStdHandle(STD_OUTPUT_HANDLE);
    if(l_pConsoleH)
    {
        COORD l_ConsoleC = { 0 };
        COORD largestSize = GetLargestConsoleWindowSize(l_pConsoleH);

        l_ConsoleC.X = (short) largestSize.X - 10;
        l_ConsoleC.Y = (short) 500;

        if(SetConsoleScreenBufferSize(l_pConsoleH, l_ConsoleC))
        {
            SMALL_RECT l_sRect = { 0 };
            l_sRect.Right = l_ConsoleC.X - 1;
            l_sRect.Bottom = 24;
            SetConsoleWindowInfo(l_pConsoleH, TRUE, &l_sRect);
        }
    }

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