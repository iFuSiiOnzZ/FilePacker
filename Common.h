#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>

char *strFileSize(unsigned int l_iFileSize, char *l_pBExt, unsigned int l_iStrSize)
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

#endif