#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// UDP 服务器测试示例

#define ERR(X) do{perror(X);exit(0);}while(0)

//定义两个宏　设置IP 与端口
//#define PORT	2022
//#define IP		"192.168.0.173"
#define IP		argv[1]
#define PORT	atoi(argv[2])

struct sockaddr_in addr;
int sockfd;

//接收	udp 消息的线程
void * threadrecvmsg(void *arg)
{
	//将输出的数据写入管道　用另一个终端显示
	FILE *fp = fopen("/home/xwq/tmp/f1", "w");
	if(fp == NULL)
		ERR("open f1 err");
	for(;;)
	{
		//recvfrom,
		//printf("正在等待接收发送给自己的UDP数据报!\n");
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
		fprintf(fp,"[%s][%d]%d字节数据了:%s\n",
					inet_ntoa(clientaddr.sin_addr),
					ntohs(clientaddr.sin_port),len,buf);
		fflush(fp);
	}
}





int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("UseAge: %s <本机IP> <端口号>\n",argv[0]);
		return -1;
	}


	printf("UDP 服务器测试示例\n");
	printf("my ip = %s port=%d\n",IP,PORT);
	// 1. 创建通信套接字
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1 )
	{
	ERR("socket err!");
	}
	printf("socket ok fd=%d\n",sockfd);
	// 2. 绑定IP端口
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

	//创建线程接收数据recvfrom
	pthread_t tid;
	if( pthread_create(&tid, NULL, threadrecvmsg, NULL) == -1){
	  ERR("pthread_create err!");}

	while(1)
	{
		struct sockaddr_in clientaddr = addr; //存放对方的IP
		char buf[100] = {0};	
		fgets(buf,sizeof(buf),stdin); //阻塞
		//192.168.0.2:2022 hello
		char * mp = strstr(buf,":");
		if(mp == NULL) 
		{
			printf("你输入的消息格式不对:例如 192.168.0.2:2022 消息\n");
			continue;
		}
		*mp = '\0';
		mp++;
		//填入IP
		inet_aton(buf,&clientaddr.sin_addr);
		//填入端口
		short port = atoi(mp);
		clientaddr.sin_port = htons(port); //端口号 网络字节序
		//消息体
		mp = strstr(mp," ");
		if(mp == NULL)
		{
			printf("你输入的消息格式不对:例如 192.168.0.2:2022 消息\n");
			continue;
		}
		printf("即将发送消息给%s,通过port=%d,内容为%s",buf,port,mp+1);

		// 给客户端　发送反馈消息
		// sendto
		sendto(sockfd, mp+1, strlen(mp+1), 0, 
					(struct sockaddr*)&clientaddr,
					sizeof(clientaddr));

	}	
	close(sockfd);
	return 0;
}

