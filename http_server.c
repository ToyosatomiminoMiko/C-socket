/**
 * @file http_server.c
 * @author ToyosatomimonoMiko
 * @brief HTTP服务器 C语言实现
 * @version 0.1
 * @date 2022-07-14
 */

#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#define ERR(X)     \
    do             \
    {              \
        perror(X); \
        exit(0);   \
    } while (0)

const char str[] = "\
HTTP/2.0 200 OK\r\n\
Server: C socket http server\r\n\
Content-Type: text/html\r\n\
\r\n\
<html><head></head>\
<body><h1>Wellcome to Phantom's web server</h1>\
</body>\
</html>";

const char image[] = "\
HTTP/1.1 200 OK\r\n\
Server: C scoket http server\r\n\
Content-Type: image/png\r\n\
\r\n\
";

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("UseAge: %s <本机IP> <端口>\n", argv[0]);
        return -1;
    }
    // 1. socket 创建流式套接字 server
    // IPv4, TCP, 非原始套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        ERR("SOCKET ERR!");
    }
    printf("socket ok!\n");

    /*
    导入 ip port 结构体
    struct sockaddr_in
    {
    u_short sin_family;  // 地址族, AF_INET，2 bytes
    u_short sin_port;     // 端口，2 bytes
    struct in_addr sin_addr; // IPV4地址，4 bytes
    char sin_zero[8];           // 8 bytes unused，作为填充 没有使用 通常填充0
    };
    */
    struct sockaddr_in addr;        // server
    memset(&addr, 0, sizeof(addr)); // 为server地址结构体分配内存
    // 填充 结构体
    addr.sin_family = AF_INET;            // 设置server协议 IPv4
    addr.sin_port = htons(atoi(argv[2])); // 设置server端口 字符串 -> 整数 -> 字节序
    // int aton = inet_aton(argv[1], &addr.sin_addr);
    if (inet_aton(argv[1], &addr.sin_addr) == 0) // 设置server地址 点分法字符串IP -> 字节序
    {
        ERR("inet_aton fail!"); // 转换失败
    }

    // 2 bind 绑定 IP, port
    // 需要绑定的套接字, 地址结构体, 结构体大小
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        ERR("BIND ERR!");
    }
    printf("IP bind ok! %s:%s\n", argv[1], argv[2]);
    // 3 linsten 使能监听 server
    // 启用监听的套接字, 最大请求数
    if (listen(sockfd, 1000) == -1)
    {
        ERR("listen err!");
    }

    // 循环服务器模型
    while (1)
    {
        char buf[1020] = {0};
        printf("服务器监听开启,正在监听连接\n");
        // 客户端信息
        struct sockaddr_in clientaddr;          // 客户端结构
        socklen_t addrlen = sizeof(clientaddr); // 客户端结构长度
        memset(&clientaddr, 0, sizeof(clientaddr));

        //等待用户连接
        // 服务器套接字, 客户端信息, 客户端结构长度
        int clientfd = accept(sockfd,
                              (struct sockaddr *)&clientaddr,
                              &addrlen);
        if (clientfd == -1)
        {
            ERR("accept err!");
        }

        printf("有客户端[%s:%d]连接了!\n",
               inet_ntoa(clientaddr.sin_addr),
               ntohs(clientaddr.sin_port));

        //接收客户端的消息

        while (1)
        {
            // 客户端套接字, 客户端消息, 客户端消息长度
            int recvlen = recv(clientfd, buf, sizeof(buf) - 1, 0);
            if (recvlen > 0)
            {
                // 客户端的消息长度超过了1020-1
                printf("收到客户端%d字节消息\n", recvlen);
                int a = sizeof(buf);
                char buf0[a];
                strcpy(buf0, buf);
                // strtok(); 按字符分割字符串
                
                char *header = strtok(buf, " ");
                for(int i=0;i<1;i++)
                {
                    header = strtok(NULL, " ");
                }
                printf("请求头:%s\n", header);
                printf("%s\n", buf0);
                memset(buf0, 0, sizeof(buf0));
                //发送数据给客户端
                // 服务器反馈: 用户连接, 反馈值, 反馈值长度

                int sendlen = send(clientfd, str, strlen(str), 0);

                if (sendlen < strlen(str))
                {
                    printf("消息没有发送完成 sendlen=%d\n", sendlen);
                }
                else if (sendlen <= 0)
                {
                    printf("消息发送失败,对方关闭了连接!\n");
                }
                close(clientfd);
            }    /*
                else if (recvlen > 0)
                {
                    // 客户端消息的最后一个部分了
                    printf("收到客户端%d字节消息:%s\n", recvlen, buf);
                    memset(buf, 0, sizeof(buf));
                    printf("用户的本次消息包接收完成!\n");
                }*/
            else // recv 返回值小于或等于0  客户端断开了连接
            {
                printf("客户端断开了连接!\n");
                break;
            }
        }
        close(clientfd);
    }

    return 0;
}

/*
®   2   4
+---+---+
1   3   5
*/
