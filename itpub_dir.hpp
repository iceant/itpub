#include<string.h>
#include <stdlib.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include<stdarg.h>
#include<sys/stat.h>
#endif

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a), 0755)
#endif


int create_dir(const char* pDir)
{
    if(NULL==pDir)
    {
        return 0;
    }
    int i=0;
    int iRet;
    int iLen;
    char * pszDir;

    pszDir = strdup(pDir);
    iLen = strlen(pszDir);

    if(pszDir[iLen-1]!='\\' && pszDir[iLen-1]!='/')
    {
        pszDir[iLen] = '/';
        pszDir[iLen+1] = '\0';
    }

    for(i=1;i<iLen; i++)
    {
        if(pszDir[i]=='\\' || pszDir[i]=='/')
        {
            pszDir[i] = '\0';
            // if not exist, create it
            iRet = ACCESS(pszDir, 0);
            if(iRet!=0)
            {
                iRet = MKDIR(pszDir);
                if(iRet!=0)
                {
                    return -1;
                }
            }
            pszDir[i]='/';
        }
    }

    free(pszDir);
    return iRet;
}
