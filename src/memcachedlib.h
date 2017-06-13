/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-12-15 14:19:33
* description：memcached客户端库
*
**********************************************************/

#ifndef  __MEMCACHEDLIB_H__
#define  __MEMCACHEDLIB_H__

#include <stdlib.h>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_KEY_LEN 255


typedef unsigned int uint32;
typedef uint32 (*Hash_gen)(const char *, size_t len);


using std::vector;
using std::map;
using std::list;
using std::shared_ptr;

//hash产生方法
enum HashGenWay
{
    MD5,    //md5 哈希方式
    CRC32,  //crc32校验码格式
};

//返回值
enum
{
    CANNT_DEAL = -20,   //无法处理错误
    NO_RESPONSE,        //服务器失联
    NOT_EXISTS,         //数据不存在
    ERROR,              //一般性错误
	CLIENT_ERROR,       //客户端错误
	SERVER_ERROR ,      //服务器错误
    NOT_STORED,         //add未存储
	NOT_FOUND= -1,      //delete未发现数据
    END,                //get正确
	STORED,             //set/add存储正确
	DELETED,            //delete成功
};

//服务器定义
struct Server
{
    ssize_t connfd;            //连接描述符
    bool connected;            //是否连接
    char host[MAX_KEY_LEN+1];  //服务器名
    char port[MAX_KEY_LEN+1];  //端口号
    vector<uint32> vhashkey;   //虚拟节点的hash值

    //构造与析构
    Server(const char *h, const char *p);
    ~Server();
};

//memcache定义
class Memcache
{
public:
    //添加或删除异常服务器
    bool add_server(const char *h, const char *p);
    void remove_server(const char *h, const char *p);
    void remove_server(shared_ptr<Server> &s);

    //增删改查
    ssize_t set(const char *key, size_t key_length, size_t flag, size_t exptime, size_t bytes, char *value);
    ssize_t add(const char *key, size_t key_length, size_t flag, size_t exptime, size_t bytes, char *value);
    ssize_t get(const char *key, size_t key_length, size_t &flag, char *value, size_t &value_length);
    ssize_t del(const char *key, size_t key_length);

    //是否存在可用服务器
    bool has_server() const { return servers.size() > 0; }
    //设置hash产生方式
    void set_hashgen(HashGenWay h);
    //设置虚服务器的个数
    void set_vnum(size_t num);

    //构造与析构
    Memcache();
    ~Memcache();

private:
    //添加虚拟服务器器
    bool add_vserver(const shared_ptr<Server> &s);
    //选择服务器
    shared_ptr<Server>  &server_select(const char *key, size_t key_length);

    //返回检查
    ssize_t check_reply(const char reply[]) const;

    size_t vnum;                                //虚拟节点个数
    Hash_gen hash_gen;                          //hash产生函数
    HashGenWay hgw;                             //hash产生方式
    static const size_t max_vnum = 1000;        //虚拟服务器最大个数
    list<shared_ptr<Server>> servers;           //服务器链表
    map<uint32, shared_ptr<Server>> vserver;    //虚拟服务器
};


#ifdef __cplusplus
}
#endif

#endif
