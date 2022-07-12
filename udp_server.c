#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// UDP 服务器测试示例

#define ERR(X) do{perror(X);exit(0);}while(0)

//定义两个宏　设置IP 与端口
#define PORT	2022
#define IP		"192.168.0.173"

int main()
{
	printf("UDP 服务器测试示例\n");
	printf("my ip = %s port=%d\n",IP,PORT);
	//创建通信套接字
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1 )
		ERR("socket err!");
	printf("socket ok fd=%d\n",sockfd);
	//绑定IP端口
	struct sockaddr_in addr;
	memset(&addr, 0 ,sizeof(addr));
	addr.sin_family = AF_INET; // IPV4 协议
	addr.sin_port = htons(PORT); //端口号 网络字节序
	//填入IP
	inet_aton(IP,&addr.sin_addr);

	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) )
	{
		ERR("bind err!");
	}
	printf("bind socket ip port ok!\n");
while(1)
{
	//recvfrom,
	printf("正在等待接收发送给自己的UDP数据报!\n");
	int len = 0;  //存放接收长度
	char buf[100] = {0}; //存放接收的数据
	struct sockaddr_in clientaddr = addr; //存放对方的IP
	socklen_t addrlen = sizeof(clientaddr);

	// 初始值通常为自己的IP

	len = recvfrom(sockfd, buf, sizeof(buf), 0,
				(struct sockaddr*)&clientaddr,
				&addrlen);
	if(len == -1)
		ERR("recvfrom err");
	printf("我收到%d字节数据了:%s\n",len,buf);
	printf("发送者的IP是 %s\n",inet_ntoa(clientaddr.sin_addr));
	// 给客户端　发送反馈消息
	// sendto
	sendto(sockfd, "RECV OK!", 8, 0, 
				(struct sockaddr*)&clientaddr,
				sizeof(clientaddr));

}	
	close(sockfd);
	return 0;
}

