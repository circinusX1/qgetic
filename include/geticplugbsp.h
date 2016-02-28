// **************************************************************************************
// Copyright (c) 2000-2005 Zalsoft Inc
// www.zalsoft.com
// sales@zalsoft.com
// You are licensed to use this file on a single computer only. 
// **************************************************************************************
#ifndef _GETIC_PLUGBSP_H__
#define _GETIC_PLUGBSP_H__
#ifdef WINDOWS
#   include <tchar.h>
#endif
#include "bspfilestr.h"

typedef enum _SEEK_POS{
    FSEEK_CUR ,
    FSEEK_END ,
    FSEEK_SET,
}SEEK_POS;

// File hook for intercepring the BSP saving data. This allows you to save the data
// in your own format.

class IBSPFileHook
{
public:
    IBSPFileHook(){}
    virtual ~IBSPFileHook(){}


    virtual int32_t    read( void *buffer, size_t size, size_t count)=0;
    virtual size_t  write( const void *buffer, size_t size, size_t count)=0;
    virtual char*   gets( char *string, int32_t n)=0;
    virtual int32_t     seek (int32_t offset, SEEK_POS origin )=0;
    virtual int32_t    tell ()=0;
    virtual int32_t     close()=0;
    virtual int32_t    wopen(const wchar_t *filename, const wchar_t *mode)=0;
    virtual int32_t    aopen(const char *filename, const char *mode)=0;
    virtual int32_t     eof( )=0;
    virtual void    start_section(const char* sectionName)=0;

};

#endif 
