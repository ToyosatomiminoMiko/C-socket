#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define ERR(X) do{perror(X);exit(0);}while(0)

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		printf("UseAge: %s <server ip> <server port>\n",argv[0]);
		return -1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) 
		ERR("socket err!");
	//SERVER ip
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2])); 
	//ip
	inet_aton(argv[1], &addr.sin_addr);
	// 发起连接请求
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	  ERR("connect err!");
	//通过sockfd 与server 通信
	
	char buf[200] = {0};
	//接收服务器的消息
	int rxlen = read(sockfd, buf, sizeof(buf)-1);
	if(rxlen > 0)
	{
		printf("recv server msg[%d bytes]: %s\n",rxlen,buf);
	}
	while(1)
	{//客户端发消息给服务器
		printf("请输入需要发送的消息\n");
		fgets(buf,sizeof(buf), stdin);
		write(sockfd, buf, strlen(buf));
	}
	return 0;
}

