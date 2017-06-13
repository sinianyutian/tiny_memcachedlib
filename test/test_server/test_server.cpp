/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-12-16 19:22:45
* description：memcached客户端测试程序
*
**********************************************************/

#include "utils.h"
#include "net.h"
#include "rio.h"
#include "md5.h"
#include "memcachedlib.h"


/*
* key/vale的get,set
* 在服务器端终止某个服务器进程，模拟服务器宕机，则客户端移除该服务器，并重新插入数据。
*/

int main(int argc, char *argv[])
{
    size_t getcount = 0;   //set计数
    size_t setcount = 10;  //get计数
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
    //不存在的服务器
    mem.add_server("127.0.0.1","11216");
    mem.add_server("127.0.0.1","11217");

    //测试字符串
    char key[] = "test";  //键
    char val[] = "value"; //值
    int  flag = 0;        //标记
    int  exptime = 10000; //有效期
    char keydata[MAXLINE];
    char valdata[MAXLINE];

    //存datanum个数据
    for(size_t i = 0; i < datanum; i++){
        //格式化key，value数据
        sprintf(keydata,"%s%lu",key,i);
        sprintf(valdata,"%s%lu",val,i);
        //发送set，如果服务器连接失败尝试其他服务器发送
        while(mem.set(keydata, strlen(keydata), flag, exptime,
                strlen(valdata), valdata) != STORED && mem.has_server()){
            fprintf(stderr, "one server is error\n");
        }
    }
    printf("set finished.\n");
    ssize_t count = 200;
    //尝试读，读不到就添加
    while(count--){
        //构造查询key值
        size_t n = count%datanum;
        sprintf(keydata,"%s%lu",key,n);
        //get内容
        size_t rflag,rlen;
        char rvalue[MAXLINE];
        //get失败(get语句正确),则表示服务器失联，已删除服务器，重新set数据
        if(mem.get(keydata, strlen(keydata), rflag, rvalue, rlen) != END){
            printf("key/value is lost.\n");
            //构造查询key/value
            sprintf(keydata,"%s%lu",key,n);
            sprintf(valdata,"%s%lu",val,n);
            setcount++; //set计数增加
            //set该查询失败数据
            while(mem.set(keydata, strlen(keydata), flag, exptime,
                strlen(valdata), valdata) != STORED && mem.has_server()){
                printf("try again.\n");
            }
        }else{
            getcount++; //get计数增加
        }
        //暂停一秒，在服务器端操作，停止某个服务器，进行测试
        sleep(1);
    }
    printf("%lu %lu\n",getcount,setcount);

    return 0;
}
