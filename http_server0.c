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
#include <fcntl.h>
#include <sys/stat.h>

//#include "lodepng.h"

/* gcc your_program.c lodepng.c -ansi -pedantic -Wall -Wextra -O3 */
/* gcc http_server.c lodepng.c  */

/*
int file_size2(char *filename)
{
    struct stat statbuf;
    stat(filename, &statbuf);
    int size = statbuf.st_size;

    return size;
}
*/
#define ERR(X)     \
    do             \
    {              \
        perror(X); \
        exit(0);   \
    } while (0)

const char str[] = "\
HTTP/1.1 200 OK\r\n\
Server: C socket http server\r\n\
Content-Type: text/html\r\n\
\r\n\
<html><head></head>\
<body><h1>Wellcome to Phantom's web server</h1>\
<img src=\"/test.png\">\
</body>\
</html>";

/*
char image[32768]="HTTP/1.1 200 OK\r\n\
Server: C scoket http server\r\n\
Content-Type: image/png\r\n\r\n";
*/
/*
int dogetcmd(int cfd)
{
    char buf[100];
    char fileName[100];
    //接收下载文件名
    read(cfd, buf, sizeof(buf) - 1);
    //sprintf(fileName, "%s%s", WORKPATH, buf);
    printf("客户端请求下载文件 %s\n", fileName);

    int fd = open(fileName, O_RDONLY);
    if (fd < 0)
    { //服务器工作目录　没有这个文件
        sprintf(fileName, "er%s文件不存放在!\n", buf);
        write(cfd, fileName, strlen(fileName));
        close(cfd);
        return 0;
    }
    //文件存在
    write(cfd, "ok", 2);
    //发送文件
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if (len <= 0)
        {
            close(cfd);
            close(fd);
            return 0;
        }
        else
        {
            if (write(cfd, buf, len) != len)
            {
                printf("发送文件错误!\n");
                close(cfd);
                close(fd);
                return -1;
            }
        }
    }
    return 0;
}
*/

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("UseAge: %s <本机IP> <端口>\n", argv[0]);
        return -1;
    }
    /* 1. socket 创建流式套接字 server */
    /* IPv4, TCP, 非原始套接字 */
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

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

    //设置套接字属性 ip port重用
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1)
    {
        ERR("setsockopt err");
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
                for (int i = 0; i < 1; i++)
                {
                    header = strtok(NULL, " ");
                }
                printf("请求路径:'%s'\n", header);
                printf("请求头:\n%s\n", buf0);
                memset(buf0, 0, sizeof(buf0));
                //发送数据给客户端
                // 服务器反馈: 用户连接, 反馈值, 反馈值长度
                int sendlen; // 响应结果
                if (!strcmp(header, "/index"))
                {
                    sendlen = send(clientfd, str, strlen(str), 0);
                    break;
                }
                else if (!strcmp(header, "/test.png"))
                {
                    struct stat file;
                    if (stat(argv[0], &file) == -1)
                    {
                        perror("stat err");
                    }
                    printf("image size:%d\n", (int)file.st_size);
                    // int pngsize= file.st_size + 128;

                    char imageheader[] = "HTTP/1.1 200 OK\r\n\
                    Server: C scoket http server\r\n\
                    Content-Type: image/png\r\n\r\n";

                    /* 发送 */

                    char buf[100];
                    char fileName[] = "./test.png";
                    //接收下载文件名
                    // read(cfd, buf, sizeof(buf) - 1);
                    // sprintf(fileName, "%s%s", WORKPATH, buf);
                    printf("客户端请求下载文件 %s\n", fileName); 

                    int fd = open(fileName, O_RDONLY);
                    if (fd < 0)
                    {
                        //服务器工作目录　没有这个文件
                        perror("文件不存在!");
                        //write(clientfd, fileName, strlen(fileName));
                        close(clientfd);
                        break;
                    }
                    //文件存在
                    printf("文件存在\n"); /* run to here */
                    write(clientfd, imageheader, sizeof(imageheader));
                    int i=0;
                    //发送文件
                    while (1)
                    {
                        memset(buf, 0, sizeof(buf));
                        int len = read(fd, buf, sizeof(buf));
                        
                        printf("i:%d\n",i++);
                        if (len <= 0)
                        {
                            
                            close(clientfd);
                            close(fd);
                            break;
                        }
                        else
                        {
                            if (write(clientfd, buf, len) != len)
                            {
                                
                                printf("发送文件错误!\n");
                                close(clientfd);
                                close(fd);
                                break;
                            }
                            printf("written\n");
                        }
                        
                    }
                    break;

                    // strcat(imageheader, image);

                    // sendlen = send(clientfd, imageheader, sizeof(imageheader), 0);
                }

                if (sendlen < strlen(str))
                {
                    printf("消息没有发送完成 sendlen=%d\n", sendlen);
                    break;
                }
                else if (sendlen <= 0)
                {
                    printf("消息发送失败,对方关闭了连接!\n");
                    break;
                }

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
®  2  4
+--+--+
1  3  5
*/
