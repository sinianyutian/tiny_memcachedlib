/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-12-16 9:34:15
* description：memcached客户端测试程序
*
**********************************************************/

#include "utils.h"
#include "net.h"
#include "rio.h"
#include "md5.h"
#include "memcachedlib.h"


/*
* 测试增删改查
*/

int main(int argc, char *argv[])
{
    size_t datanum = 30;   //测试数据数量

    //创建memcache客户端
    Memcache mem;

    //添加服务器
    //存在的服务器
    mem.add_server("127.0.0.1","11211");
    mem.add_server("127.0.0.1","11212");
    mem.add_server("127.0.0.1","11213");
    mem.add_server("127.0.0.1","11214");
    mem.add_server("127.0.0.1","11215");
    //该服务器并不存在
    mem.add_server("127.0.0.1","11216");
    mem.add_server("127.0.0.1","11217");

    //测试字符串
    char key[] = "test";  //键
    char val[] = "value"; //值
    int  flag = 0;        //标记
    int  exptime = 10000; //有效期
    char keydata[MAXLINE];
    char valdata[MAXLINE];

    //set测试
    for(size_t i = 0; i < datanum; i++){
        //格式化key，value数据
        sprintf(keydata,"%s%lu",key,i);
        sprintf(valdata,"%s%lu",val,i);
        //发送set，如果未存储成功，尝试其他服务器发送
        while(mem.set(keydata, strlen(keydata), flag+i, exptime,
                strlen(valdata), valdata) != STORED && mem.has_server()){
            fprintf(stderr, "one server is error\n");
        }
    }
    printf("set finished.\n");

    //add测试
    for(size_t i = 0; i < datanum; i++){
        //格式化key，value数据
        sprintf(keydata,"%s-%lu",key,i);
        sprintf(valdata,"%s-%lu",val,i);
        //发送add，如果服务器连接失败尝试其他服务器发送
        while(mem.add(keydata, strlen(keydata), flag+i, exptime,
                strlen(valdata), valdata) == NO_RESPONSE && mem.has_server()){
            fprintf(stderr, "one server is error\n");
        }
    }
    printf("add finished.\n");

    //get测试
    for(size_t i = 0; i < datanum; i++){
        //格式化key
        sprintf(keydata,"%s%lu",key,i);
        //get内容
        size_t rflag,rlen;
        char rvalue[MAXLINE];
        //发送get
        if(mem.get(keydata, strlen(keydata), rflag, rvalue, rlen) != END){
            printf("get error.\n");
        }else{
            printf("[k,v]:(%s,%s) flag:%lu\n",keydata,rvalue,rflag);
        }

    }
    printf("get finished.\n");

    //delete测试
    for(size_t i = 0; i < datanum; i++){
        //格式化key
        sprintf(keydata,"%s-%lu",key,i);
        //发送delete
        if(mem.del(keydata, strlen(keydata)) != DELETED){
            printf("delete error.\n");
        }

    }
    printf("delete finished.\n");

    return 0;
}
