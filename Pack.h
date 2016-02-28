#ifndef _WRITETREE_H_
#define _WRITETREE_H_

#include "DirData.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char *strFileSize1(unsigned int l_iFileSize, char *l_pBExt, unsigned int l_iStrSize)
{
     char l_pSize[256] = { 0 }; 
     sprintf_s(l_pSize, "%d", l_iFileSize);
     sprintf_s(l_pBExt, l_iStrSize, "%5s KB", l_pSize);

    if(l_iFileSize > 1024)
    {
        l_iFileSize /= 1024;
        sprintf_s(l_pSize, "%d", l_iFileSize);
        sprintf_s(l_pBExt, l_iStrSize, "%5s KB", l_pSize);
    }

    if(l_iFileSize > 1024)
    {
        l_iFileSize /= 1024;
        sprintf_s(l_pSize, "%d", l_iFileSize);
        sprintf_s(l_pBExt, l_iStrSize, "%5s MB", l_pSize);
    }

    if(l_iFileSize > 1024)
    {
        l_iFileSize /= 1024;
        sprintf_s(l_pSize, "%d", l_iFileSize);
        sprintf_s(l_pBExt, l_iStrSize, "%5s GB", l_pSize);
    }

    return l_pBExt;
}


void PackFile(S_PFILEDATA l_pFileData, FILE *l_pFile, unsigned int l_NumFiles)
{
    char l_pBExt[32] = { 0 };

    for(unsigned int i = 0; i < l_NumFiles; i++, l_pFileData = l_pFileData->Next)
    {
        printf("Packing: (%s) %160s  %5s", strFileSize1(l_pFileData->FileSize, l_pBExt, 32), l_pFileData->Name, "...");

        char *l_pFileName = strrchr(l_pFileData->Name, '\\');

        if(!l_pFileName) l_pFileName = l_pFileData->Name;
        else l_pFileName++;

        fwrite(l_pFileName, sizeof(char), MAX_PATH, l_pFile);
        fwrite(&l_pFileData->FileSize, sizeof(unsigned int), 1, l_pFile);

        FILE *l_pFile2Read = NULL;
        fopen_s(&l_pFile2Read, l_pFileData->Name, "rb");

        char l_pBuffer[1024] = { 0 };
        unsigned int r = 0, w = 0;

        while(!feof(l_pFile2Read))
        {
            unsigned int l = fread(l_pBuffer, sizeof(char), 1024, l_pFile2Read);
            w += fwrite(l_pBuffer, sizeof(char), l, l_pFile);
            r += l;
        }

        assert(r == l_pFileData->FileSize);
        assert(w == r); 
        
        fclose(l_pFile2Read);
        printf("\t\t DONE\n");
    }

}

void PackFolder(S_PDIRDATA l_pDirData, FILE *l_pFile)
{
    fwrite(&l_pDirData->NumFiles, sizeof(unsigned int), 1, l_pFile);
    fwrite(&l_pDirData->NumDirs, sizeof(unsigned int), 1, l_pFile);

    char *l_pDirName = strchr(l_pDirData->Name, '\\');
    char l_pActualDir[4] = "";

    if(!l_pDirName) l_pDirName = l_pActualDir;
    else l_pDirName++;

    fwrite(l_pDirName, sizeof(char), MAX_PATH, l_pFile);

    PackFile(l_pDirData->Files, l_pFile, l_pDirData->NumFiles);
    for(unsigned int i = 0; i < l_pDirData->NumDirs; i++)
    {
        PackFolder(l_pDirData->Next[i], l_pFile);
    }
}

#endif