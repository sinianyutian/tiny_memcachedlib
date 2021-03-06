/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-5-5 12:08:10
* description：常用工具函数的定义，from CSAPP
*
**********************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int uint32;
typedef unsigned short uint16;

#define MAXLINE 1024

// libc定义的环境变量
extern char **environ;

//unix错误，退出
void error_exit(const char *msg);
//应用程序错误
void app_error(char *msg);

//异步信号安全输出
void Sio_put(const char *str);

//i/o基本操作
int Open(const char *pathname, int flags, mode_t mode);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
off_t Lseek(int fildes, off_t offset, int whence);
void Close(int fd);

//标准i/o
void Fclose(FILE *fp);
FILE *Fdopen(int fd, const char *type);
char *Fgets(char *ptr, int n, FILE *stream);
FILE *Fopen(const char *filename, const char *mode);
void Fputs(const char *ptr, FILE *stream);
size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

#ifdef __cplusplus
}
#endif
#endif
