#ifndef __CCRC_H__
#define __CCRC_H__

#include <baselib.h>

class CCrc
{
public:
    CCrc(u_int32_t poly =  0x04c11db7L);
    virtual ~CCrc();
    u_int32_t update_crc(u_int32_t crc_accum, u_int8_t *data_blk_ptr, int32_t data_blk_size);

    u_int32_t           polynomial;

    u_int32_t   crc_table[256];

};

#endif //
