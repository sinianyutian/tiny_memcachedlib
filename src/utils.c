/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-6-5 12:08:10
* description：常用工具函数的实现，from CSAPP
*
**********************************************************/

#include "utils.h"

//unix输出错误
void error_exit(const char *msg)
{
    fprintf(stderr, "%s error: %s.\n", msg ,strerror(errno));
    exit(0);
}
//应用程序错误
void app_error(char *msg)
{
    fprintf(stderr, "%s.\n", msg);
    exit(0);
}

//异步信号安全输出,调用函数必须均为异步信号安全函数
void Sio_put(const char *str)
{
    if(write(STDOUT_FILENO, str, strlen(str)) < 0){
        _exit(1);
    }
}

//i/o基本操作
int Open(const char *pathname, int flags, mode_t mode)
{
    int rc;

    if ((rc = open(pathname, flags, mode))  < 0){
	       error_exit("Open");
    }
    return rc;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    ssize_t rc;

    if ((rc = read(fd, buf, count)) < 0){
         error_exit("Read");
    }
    return rc;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    ssize_t rc;

    if ((rc = write(fd, buf, count)) < 0){
        error_exit("Write");
    }
    return rc;
}

off_t Lseek(int fildes, off_t offset, int whence)
{
    off_t rc;

    if ((rc = lseek(fildes, offset, whence)) < 0){
        error_exit("Lseek");
    }
    return rc;
}

void Close(int fd)
{
    if ((close(fd)) < 0){
        error_exit("Close");
    }
}

int Select(int  n, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout)
{
    int rc;

    if ((rc = select(n, readfds, writefds, exceptfds, timeout)) < 0){
        error_exit("Select");
    }
    return rc;
}

//标准库i/o
void Fclose(FILE *fp)
{
    if (fclose(fp) != 0){
        error_exit("Fclose");
    }
}

FILE *Fdopen(int fd, const char *type)
{
    FILE *fp;

    if ((fp = fdopen(fd, type)) == NULL){
        error_exit("Fdopen");
    }
    return fp;
}

char *Fgets(char *ptr, int n, FILE *stream)
{
    char *rptr;

    if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream)){
        app_error("Fgets");
    }

    return rptr;
}

FILE *Fopen(const char *filename, const char *mode)
{
    FILE *fp;

    if ((fp = fopen(filename, mode)) == NULL){
        error_exit("Fopen");
    }

    return fp;
}

void Fputs(const char *ptr, FILE *stream)
{
    if (fputs(ptr, stream) == EOF){
        error_exit("Fputs");
    }
}

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t n;

    if (((n = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream)){
        error_exit("Fread");
    }

    return n;
}

void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (fwrite(ptr, size, nmemb, stream) < nmemb){
        error_exit("Fwrite");
    }
}
