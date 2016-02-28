//----------------------------------------------------------------------------------------
// Unauthorized use or duplication is strictly prohibited.
// Copyright Zalsoft Inc 1999-2004.
//// Author: Marius C of Zalsoft Inc 2000-2004
//----------------------------------------------------------------------------------------
#include "stdafx.h"
#include "crck.h"


//--| poly|------------------------------------------------------------------------
CCrc::CCrc(u_int32_t poly):polynomial(poly)
{
    int32_t i, j;
    u_int32_t crc_accum;
    for ( i = 0;  i < 256;  i++ )
    {
        crc_accum = ( (u_int32_t) i << 24 );
        for ( j = 0;  j < 8;  j++ )
        {
            if ( crc_accum & 0x80000000 )
               crc_accum =( crc_accum << 1 ) ^ polynomial;
            else
                crc_accum = ( crc_accum << 1 );
        }
        crc_table[i] = crc_accum;
    }
}

//--|CCrc::~CCrc|------------------------------------------------------------------------
CCrc::~CCrc()
{

}


//--| CCrc::update_crc|-------------------------------------------------------------------
u_int32_t CCrc::update_crc(u_int32_t crc_accum, u_int8_t *data_blk_ptr, int32_t data_blk_size)
{
    int32_t i, j;
    for ( j = 0;  j < data_blk_size;  j++ )
    {
        i = ( (int32_t) ( crc_accum >> 24) ^ *data_blk_ptr++ ) & 0xff;
        crc_accum = ( crc_accum << 8 ) ^ crc_table[i];
    }
    return crc_accum;
}
