#include <iconv.h>

int  utf8_to_gbk( const   char  *sourcebuf, size_t  sourcelen, char  *destbuf, size_t  destlen )
{
    iconv_t cd;
    if ( (cd = iconv_open( "gbk" , "utf-8"  )) ==0  )
        return  -1;
    memset(destbuf,0,destlen);
    const   char  **source = &sourcebuf;
    char  **dest = &destbuf;
    if (-1 == iconv(cd,source,&sourcelen,dest,&destlen))
        return  -1;
    iconv_close(cd);
    return  0;
} 

int  gbk_to_utf8( const   char  *sourcebuf, size_t  sourcelen, char  *destbuf, size_t  destlen )
{
    iconv_t cd;
    if ( (cd = iconv_open( "utf-8" , "gbk"  )) ==0  )
        return  -1;
    memset(destbuf,0,destlen);
    const   char  **source = &sourcebuf;
    char  **dest = &destbuf;
    if (-1 == iconv(cd,source,&sourcelen,dest,&destlen))
        return  -1;
    iconv_close(cd);
    return  0;
} 
