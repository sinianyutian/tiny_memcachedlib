/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-12-16 10:9:24
* description：crc32产生hash函数，from Spencer Garrett <srg@quick.com>
*
**********************************************************/

#ifndef  __CRC32_H__
#define  __CRC32_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int uint32_t;

//crc32产生hash函数
uint32_t hash_crc32(const char *key, size_t key_length);


#ifdef __cplusplus
}
#endif

#endif
