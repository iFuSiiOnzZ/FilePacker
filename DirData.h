#ifndef _DIRDATA_H_
#define _DIRDATA_H_

#include <sys/stat.h>
#include <Windows.h>

#define CHECKED_DELETE(x)       { if(x != NULL){ delete x; x = NULL; } }
#define CHECKED_DELETE_ARRAY(x) { if(x != NULL){ delete [] x; x = NULL; } }

typedef struct FILEDATA
{
    char Name[MAX_PATH];
    unsigned int FileSize;
    struct FILEDATA *Next;
} S_FILEDATA, *S_PFILEDATA;

typedef struct DIRDATA
{
    char Name[MAX_PATH];
    unsigned int NumDirs;
    unsigned int NumFiles;

    S_PFILEDATA Files;
    struct DIRDATA *Next[256];
} S_DIRDATA, *S_PDIRDATA;


unsigned int GetFileSize(const char *l_pFileName)
{
    struct stat stat_buf;
    int rc = stat(l_pFileName, &stat_buf);

    return rc == 0 ? stat_buf.st_size : 0;
}

S_PDIRDATA ListDir(const char *l_pDirPath)
{
    char l_pDir[MAX_PATH] = { 0 };
    sprintf_s(l_pDir, "%s\\*", l_pDirPath);

    WIN32_FIND_DATAA l_DirData;
    HANDLE l_pDirHandle = FindFirstFileA(l_pDir, &l_DirData);

    if(l_pDirHandle == INVALID_HANDLE_VALUE) return NULL;
    unsigned int l_NumCurrentDir = 0;

    S_PDIRDATA l_psDirData = (S_PDIRDATA) malloc (sizeof(S_DIRDATA));
    if(l_psDirData == NULL) {FindClose(l_pDirHandle); return NULL; }

    memset(l_psDirData, 0, sizeof(S_DIRDATA));
    S_PDIRDATA l_psNextDir = l_psDirData;

    strcpy_s(l_psNextDir->Name, l_pDirPath);
    S_PFILEDATA l_pFileNext = l_psNextDir->Files;

    do 
    {
        const char *l_pName = l_DirData.cFileName;
        if(!strcmp(l_pName, ".") || !strcmp(l_pName, "..")) continue;

        char l_pNewDir[MAX_PATH] = { 0 };
        sprintf_s(l_pNewDir, "%s\\%s", l_pDirPath, l_pName);

        if (l_DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            l_psNextDir->Next[l_NumCurrentDir] = ListDir(l_pNewDir);
            l_NumCurrentDir++;
        }
        else
        {
            if(!l_pFileNext) 
            {
                l_psNextDir->Files = (S_PFILEDATA) malloc(sizeof(S_FILEDATA));
                memset(l_psNextDir->Files, 0, sizeof(S_FILEDATA));
                l_pFileNext = l_psNextDir->Files;
            }
            else
            {
                l_pFileNext->Next= (S_PFILEDATA) malloc(sizeof(S_FILEDATA));
                memset(l_pFileNext->Next, 0, sizeof(S_FILEDATA));
                l_pFileNext = l_pFileNext->Next;
            }

            l_psNextDir->NumFiles++;
            l_pFileNext->FileSize = GetFileSize(l_pNewDir);

            strcpy_s(l_pFileNext->Name, l_pNewDir);
            l_pFileNext->Next = NULL;
        }

    } while (FindNextFileA(l_pDirHandle, &l_DirData));

    FindClose(l_pDirHandle);

    l_psDirData->NumDirs = l_NumCurrentDir;
    return l_psDirData;
}

void DeleteFileData(S_PFILEDATA l_pFile)
{
    while(l_pFile)
    {
        S_PFILEDATA d = l_pFile;
        l_pFile = l_pFile->Next;
        CHECKED_DELETE(d);
    }
}

void DeleteDirData(S_PDIRDATA l_pDir)
{
    if(!l_pDir) return;
    DeleteFileData(l_pDir->Files);

    for(unsigned int i = 0; i < l_pDir->NumDirs; i++)
    {
        DeleteDirData(l_pDir->Next[i]);
        CHECKED_DELETE(l_pDir->Next[i]);
    }
}

void CreateFolder(char *l_pFolder)
{
    char *l_pFound = strchr(l_pFolder, '\\');
    char l_pNextFolder[MAX_PATH] = { 0 };

    while(l_pFound)
    {
        strncpy_s(l_pNextFolder, l_pFolder, l_pFound - l_pFolder);
        CreateDirectoryA(l_pNextFolder, NULL);
        l_pFound = strchr(l_pFound + 1, '\\');
    }

    CreateDirectoryA(l_pFolder, NULL);
}

#endif