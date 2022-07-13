#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define ERR(X) do{perror(X);exit(0);}while(0)

const char str[] = "欢迎连接服务器!\n";


int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		printf("UseAge: %s <本机IP> <端口>\n",argv[0]);
		return -1;
	}
	//1 socket 创建流式套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
		ERR("SOCKET ERR!");
	printf("socket ok!\n");
	//2 bind
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	//填充ip port 
	addr.sin_family = AF_INET;
	addr.sin_port = htons( atoi(argv[2]));

	inet_aton(argv[1],&addr.sin_addr);
	
	if( bind(sockfd, (struct sockaddr*)&addr,sizeof(addr)) == -1 )
		ERR("BIND ERR!");
	printf("IP bind ok! %s:%s\n",argv[1],argv[2]);
	//3 linsten 使能监听
	if( listen(sockfd, 1000) == -1  )
		ERR("listen err!");

	//循环服务器模型
	while(1)
	{
		char buf[200] = {0};
		printf("服务器监听开启,正在监听连接\n");
		struct sockaddr_in clientaddr;
		socklen_t addrlen = sizeof(clientaddr);
		memset(&clientaddr, 0, sizeof(clientaddr));
		//等待用户连接
		int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, 
					&addrlen);
		if(clientfd == -1)
			ERR("accept err!");
		printf("有客户端[%s:%d]连接了!\n",inet_ntoa(clientaddr.sin_addr), 
					ntohs(clientaddr.sin_port));
		//发送数据给客户端
		int sendlen = write(clientfd , str, strlen(str));
		if(sendlen < strlen(str))
		{
			printf("消息没有发送完成 sendlen=%d\n", sendlen);
		}
		else if(sendlen <= 0)
		{
			printf("消息发送失败,对方关闭了连接!\n");
		}
		//接收客户端的消息
		while(1)
		{
			int readlen = read(clientfd, buf,sizeof(buf)-1);
			if(readlen == sizeof(buf)-1 )
			{// 客户端的消息长度超过了200-1

				printf("收到客户端%d字节消息:%s\n",readlen, buf);
				memset(buf,0,sizeof(buf));
				continue;
			}
			else if (readlen > 0 )
			{// 客户端消息的最后一个部分了
				printf("收到客户端%d字节消息:%s\n",readlen, buf);
				memset(buf,0,sizeof(buf));
				printf("用户的本次消息包接收完成!\n");	
			}
			else  // read 返回值小于或等于0  客户端断开了连接
			{
				printf("客户端断开了连接!\n");
				break;
			}
		}
		close(clientfd);
	}


	return 0;
}


