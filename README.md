# tiny_memcachedlib
一个简单的memcached的C++客户端库,可以实现与memcached的通信。

- [x] 单进程与memcached服务器进行通信
- [x] 可动态地添加和删除memcached服务器
- [x] 一致性hash协议(可设置虚节点个数)
- [x] Key/Value的增删改查(set,get,add,delete)

---
#### 文件目录说明
    src：库的源码目录
    test：两个测试程序
    create_memcached：一个创建memcached服务的脚本（首先已经安装memcached）

---

#### 感谢
1. md5、crc32使用了开源代码，已在代码中说明
2. 网络通信和I/O函数的封装参考了CSAPP
3. 此外，一些博客上分享，让我快速的了解了memcached基本原理，通信接口、通信协议、分布式一致性hash协议等内容，感谢！！！

更详细的说明请参照我的博客：www.codefarmer.me
