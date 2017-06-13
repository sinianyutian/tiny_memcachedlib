/**********************************************************
*
* Author：　　　ShooterIT
* Datatime: 　　2016-12-15 14:22:18
* description：memcached客户端库
*
**********************************************************/

#include "utils.h"
#include "rio.h"
#include "net.h"
#include "md5.h"
#include "crc32.h"
#include "memcachedlib.h"

//服务器构造函数
Server::Server(const char *h, const char *p):connfd(-1),connected(false)
{
    strncpy(host,h,MAX_KEY_LEN); //主机
    host[MAX_KEY_LEN] = '\0';
    strncpy(port,p,MAX_KEY_LEN); //端口号
    port[MAX_KEY_LEN] = '\0';
}

//服务器析构
Server::~Server()
{
    //断开连接
    if(connected){
        Close(connfd);
    }
}

//memcached构造函数
Memcache::Memcache():vnum(100),hgw(MD5)
{
    //hash函数产生方式
    switch (hgw) {
        case MD5:{
            hash_gen = &md5uint32;
            break;
        }
        case CRC32:{
            hash_gen = &hash_crc32;
            break;
        }
        default:
            hash_gen = &md5uint32;
    }
}

//memcache的析构
Memcache::~Memcache()
{
    //清理所有服务器
    servers.clear();

    //清理虚拟服务器
    vserver.clear();
}

//设置虚拟服务器的个数
void Memcache::set_vnum(size_t num)
{
    //如果数目超过上限，置为最大max
    if(max_vnum < num){
        num = max_vnum;
    }
    //设置虚拟服务器数目
    vnum = num;
}

//添加虚拟服务器
bool Memcache::add_vserver(const shared_ptr<Server> &s)
{
    char buf[MAXLINE+1];
    for(size_t i = 0; i < vnum; i++){
        //生成虚拟服务器字符串：如192.168.0.1:11211#1,192.168.0.1:11211#2
        sprintf(buf,"%s:%s#%lu",s->host, s->port, i);
        uint32 hashkey = hash_gen(buf,strlen(buf));
        //虚拟服务器记录真正的服务器
        vserver[hashkey] = s;
        //真正的服务器记录虚拟主机的hash值，以便删除真正服务器是删除虚拟主机
        (s->vhashkey).push_back(hashkey);
    }
    return true;
}

//计算服务器位置
shared_ptr<Server> &Memcache::server_select(const char *key, size_t key_length)
{
    //计算key的hash值
    uint32 hashkey = hash_gen(key,key_length);
    //根据hash一直性协议，确定该key放置位置，第一个不小于该key为其位置
    auto it =  vserver.lower_bound(hashkey);
    //环状，为查找到就认识是第一个
    if(it == vserver.end()){
        it = vserver.begin();
    }
    return it->second;
}

//添加服务器
bool Memcache::add_server(const char *h, const char *p)
{
    //创建服务器
    shared_ptr<Server> s = std::make_shared<Server>(h,p);

    //插入服务器列表
    servers.push_back(s);

    //添加虚拟节点
    add_vserver(s);

    return true;
}

//移除服务器,同时移除虚拟主机
void Memcache::remove_server(const char *h, const char *p)
{
    //查找server，并删除
    for(auto it = servers.begin(); it != servers.end(); it++){
        if(!strcmp((*it)->host,h) && !strcmp((*it)->port,h)){

            //删除虚拟服务器
            for(uint32 k : (*it)->vhashkey){
                vserver.erase(k);
            }

            //删除服务器
            servers.erase(it);
            break;
        }
    }
}

//删除服务器
void Memcache::remove_server(shared_ptr<Server> &s)
{
    //删除虚拟服务器
    for(uint32 k : s->vhashkey){
        vserver.erase(k);
    }
    //删除服务器
    servers.remove(s);
}

ssize_t Memcache::check_reply(const char reply[]) const
{
    if(!strcmp(reply,"END\r\n")) return END;
    if(!strcmp(reply,"STORED\r\n")) return STORED;
    if(!strcmp(reply,"NOT_STORED\r\n")) return NOT_STORED;
	if(!strcmp(reply,"NOT_FOUND\r\n")) return NOT_FOUND;
	if(!strcmp(reply,"DELETED\r\n")) return DELETED;
    if(!strcmp(reply,"ERROR\r\n")) return ERROR;
	if(!strncmp(reply,"CLIENT_ERROR",strlen("CLIENT_ERROR"))) return CLIENT_ERROR;
    if(!strncmp(reply,"SERVER_ERROR",strlen("SERVER_ERROR"))) return SERVER_ERROR;
    return CANNT_DEAL;
}

//制作标准字符串
static void make_string(char *buf, const char *data, size_t data_length, size_t max_length)
{
    //长度截取
    if(data_length > max_length){
        data_length = max_length;
    }
    //拷贝data
    strncpy(buf, data, data_length);
    buf[data_length] = '\0';
}

//服务器失联则删除断开
#define no_response_disconnect(n) do{ \
        if(n <= 0){ \
            remove_server(s); \
            return NO_RESPONSE;\
        }\
    }while(0)

//连接服务器，如果没有连接则连接，连接失败则删除服务器
#define conect_server(s) do{\
            if(!s->connected){\
                ssize_t fd = open_clientfd(s->host, s->port);\
                if(fd < 0){\
                    remove_server(s);\
                    return NO_RESPONSE;\
                }\
                s->connfd = fd;\
                s->connected = true;\
            }\
        }while (0);

//设置key,value
ssize_t Memcache::set(const char *key, size_t key_length, size_t flag, size_t exptime, size_t bytes, char *value)
{
    char buf[MAXLINE+1];

    //拷贝key
    char keydata[MAX_KEY_LEN+1];
    make_string(keydata,key,key_length,MAX_KEY_LEN);

    //拷贝value
    char valuedata[MAXLINE+1];
    make_string(valuedata,value,bytes,MAXLINE);

    //计算服务器的位置
    shared_ptr<Server> s = server_select(keydata, key_length);

    //连接服务器
    conect_server(s);

    //发送数据
    size_t clientfd = s->connfd;
    //初始化io缓存
    rio_t rio;
    rio_initbuf(&rio,clientfd);
    sprintf(buf, "set %s %lu %lu %lu\r\n%s\r\n",keydata,flag, exptime, bytes, valuedata);
    //写数据
    int nwrite = rio_write(clientfd, buf, strlen(buf));
    no_response_disconnect(nwrite);//读写失败，删除服务器

    //读结果
    ssize_t nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread);//读写失败，删除服务器
    buf[nread] = '\0';

    //返回结果
    ssize_t ret = check_reply(buf);
    //指令错误
    if(ret == ERROR){
        return ERROR;
    }
    //客户端错误
    if(ret == CLIENT_ERROR){
        //再读错误
        nread = rio_readlineb(&rio, buf, MAXLINE);
        no_response_disconnect(nread);//读写失败，删除服务器
        buf[nread] = '\0';
        if (check_reply(buf) != ERROR) {
            return CANNT_DEAL;
        }
    }
    //服务器错误，断开该服务器
    if(ret == SERVER_ERROR){
        remove_server(s);
        return NO_RESPONSE;
    }
    return ret;
}

//添加key,value
ssize_t Memcache::add(const char *key, size_t key_length, size_t flag, size_t exptime, size_t bytes, char *value)
{
    char buf[MAXLINE+1];

    //拷贝key
    char keydata[MAX_KEY_LEN+1];
    make_string(keydata,key,key_length,MAX_KEY_LEN);

    //拷贝value
    char valuedata[MAXLINE+1];
    make_string(valuedata,value,bytes,MAXLINE);

    //计算服务器的位置
    shared_ptr<Server> s = server_select(keydata, key_length);

    //连接服务器
    conect_server(s);

    //发送数据
    size_t clientfd = s->connfd;
    rio_t rio;
    rio_initbuf(&rio,clientfd);
    sprintf(buf, "add %s %lu %lu %lu\r\n%s\r\n",keydata,flag, exptime, bytes, valuedata);
    int nwrite = rio_write(clientfd, buf, strlen(buf));
    no_response_disconnect(nwrite);//读写失败，删除服务器

    //读结果
    ssize_t nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread);//读写失败，删除服务器
    buf[nread] = '\0';

    //返回结果
    return check_reply(buf);
}

//查询key
ssize_t Memcache::get(const char *key, size_t key_length, size_t &flag, char *value, size_t &value_length)
{
    char buf[MAXLINE+1];

    //拷贝key
    char keydata[MAX_KEY_LEN+1];
    make_string(keydata,key,key_length,MAX_KEY_LEN);

    //计算服务器的位置
    shared_ptr<Server> s = server_select(keydata, key_length);

    //连接服务器
    conect_server(s);

    //发送数据
    ssize_t clientfd = s->connfd;
    rio_t rio;
    rio_initbuf(&rio,clientfd);
    sprintf(buf, "get %s\r\n",keydata);
    int nwrite = rio_write(clientfd, buf, strlen(buf));
    no_response_disconnect(nwrite);//读写失败，删除服务器

    //读结果
    ssize_t nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread); //读写失败，删除服务器
    buf[nread] = '\0';
    ssize_t ret = check_reply(buf);
    //数据不存在
    if(ret == END){
        return NOT_EXISTS;
    }
    //指令错误
    if(ret == ERROR){
        return ERROR;
    }

    //解析 VALUE <key> <flag> <bytes>
    sscanf(buf, "%*s %*s %lu %lu",&flag,&value_length);


    //读value
    nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread);//读写失败，删除服务器
    buf[nread] = '\0';
    sscanf(buf, "%s",value);

    //读结果
    nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread);//读写失败，删除服务器
    buf[nread] = '\0';
    ret = check_reply(buf);
    if(ret != END){
        return CANNT_DEAL;
    }
    return ret;
}

//删除key
ssize_t Memcache::del(const char *key, size_t key_length)
{
    char buf[MAXLINE+1];

    //拷贝key
    char keydata[MAX_KEY_LEN+1];
    make_string(keydata,key,key_length,MAX_KEY_LEN);

    //计算服务器的位置
    shared_ptr<Server> s = server_select(keydata, key_length);

    //连接服务器
    conect_server(s);

    //发送数据
    ssize_t clientfd = s->connfd;
    rio_t rio;
    rio_initbuf(&rio,clientfd);
    sprintf(buf, "delete %s\r\n",keydata);
    int nwrite = rio_write(clientfd, buf, strlen(buf));
    no_response_disconnect(nwrite);//读写失败，删除服务器

    //读结果
    ssize_t nread = rio_readlineb(&rio, buf, MAXLINE);
    no_response_disconnect(nread); //读写失败，删除服务器
    buf[nread] = '\0';

    return check_reply(buf);
}
