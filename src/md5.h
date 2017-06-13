/**********************************************************
*
* Author：　　　ulwanski
* modified:    ShooterIT
* description：md5计算，来自https://github.com/ulwanski/md5
*
**********************************************************/

#ifndef MD5_H
#define MD5_H

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstring>
#include <cstdio>

typedef unsigned int uint32;

std::string md5(std::string dat);
std::string md5(const void* dat, size_t len);
std::string md5file(const char* filename);
std::string md5file(FILE* file);
std::string md5sum6(std::string dat);
std::string md5sum6(const void* dat, size_t len);
uint32      md5uint32(const char* dat, size_t len);
void        md5bin(const void* dat, size_t len, unsigned char out[16]);

#endif // end of MD5_H
