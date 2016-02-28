#ifndef _READTREE_H_
#define _READTREE_H_

#include "DirData.h"
#include <assert.h>
#include  <io.h>

bool FolderExists(const char* l_pFolder)
{
    if (_access(l_pFolder, 0) == -1) 
    {
        return false;
    }

    DWORD attr = GetFileAttributesA(l_pFolder);

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    return false;
}

char *strFileSize2(unsigned int l_iFileSize, char *l_pBExt, unsigned int l_iStrSize)
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

S_PFILEDATA UnpackFiles(FILE *l_pFile, unsigned int l_NumFiles, char *l_pFolder)
{
    if(!l_NumFiles) return NULL;

    S_PFILEDATA l_pFileData = (S_PFILEDATA) malloc (sizeof(S_FILEDATA));
    memset(l_pFileData, 0, sizeof(S_FILEDATA));
    S_PFILEDATA l_pFileCurrent = l_pFileData;

    char l_pBExt[32] = { 0 };
    for(unsigned int i = 0; i < l_NumFiles; i++)
    {
        fread(l_pFileCurrent->Name, sizeof(char), MAX_PATH, l_pFile);
        fread(&l_pFileCurrent->FileSize, sizeof(unsigned int), 1, l_pFile);

        char l_pUnpackPath[256] = { 0 };
        sprintf_s(l_pUnpackPath, "%s\\%s", l_pFolder, l_pFileCurrent->Name);
        printf("Unpacking: (%s) %160s %5s", strFileSize2(l_pFileCurrent->FileSize, l_pBExt, 32), l_pUnpackPath, "...");

      
        char l_pBuffer[1024] = { 0 };
        unsigned int l_BytesReaded = 0;
        unsigned int l_BytesToRead = 1024 > l_pFileCurrent->FileSize ? l_pFileCurrent->FileSize : 1024;

        FILE *l_pFile2Write = NULL;
        fopen_s(&l_pFile2Write, l_pUnpackPath, "wb");

        while(l_BytesReaded < l_pFileCurrent->FileSize && l_BytesToRead < l_pFileCurrent->FileSize - l_BytesReaded)
        {
            unsigned int r = fread(l_pBuffer, 1, l_BytesToRead, l_pFile);
            fwrite(l_pBuffer, 1, r, l_pFile2Write);
            l_BytesReaded += r;
        }

        unsigned int r = fread(l_pBuffer, 1, l_pFileCurrent->FileSize - l_BytesReaded, l_pFile);
        fwrite(l_pBuffer, 1, r, l_pFile2Write);

        fclose(l_pFile2Write);
        printf("\t\t DONE\n");

        l_pFileCurrent->Next = (S_PFILEDATA) malloc (sizeof(S_FILEDATA));
        memset(l_pFileCurrent->Next, 0, sizeof(S_FILEDATA));
        l_pFileCurrent = l_pFileCurrent->Next;
    }

    return l_pFileData;
}

S_PDIRDATA UnpackFolder(FILE *l_pFile, char *l_pFolder)
{
    S_PDIRDATA l_pFolderData = (S_PDIRDATA) malloc (sizeof(S_DIRDATA));
    memset(l_pFolderData, 0, sizeof(S_DIRDATA));

    fread(&l_pFolderData->NumFiles, sizeof(unsigned int), 1, l_pFile);
    fread(&l_pFolderData->NumDirs, sizeof(unsigned int), 1, l_pFile);
    fread(l_pFolderData->Name, sizeof(char), MAX_PATH, l_pFile);

    char l_pUnpackPath[MAX_PATH] = { 0 };
    sprintf_s(l_pUnpackPath, "%s\\%s", l_pFolder, l_pFolderData->Name);

    CreateFolder(l_pUnpackPath);
    l_pFolderData->Files = UnpackFiles(l_pFile, l_pFolderData->NumFiles, l_pUnpackPath);
    for(unsigned int i = 0; i < l_pFolderData->NumDirs; i++)
    {
        l_pFolderData->Next[i] = UnpackFolder(l_pFile, l_pFolder);
    }

    return l_pFolderData;
}

#endif