#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

//主要参考资料http://pubs.opengroup.org
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 5272 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

int main (int argc, char **argv)
{
	int listenfd, connfd, n;
	//<sys/socket.h>定义的socklen_t数据类型，不能多于2^32-1
	socklen_t clilen;
	char buf[MAXLINE];
	//<netinet/in.h>定义的结构体sockaddr_in,在内存中和struct sockaddr基本一致,指针似乎可以互换。
		//struct sockaddr_in {
		//    short            sin_family;   // e.g. AF_INET
		//        unsigned short   sin_port;     // e.g. htons(3490)
		//            struct in_addr   sin_addr;     // see struct in_addr, below
		//                char             sin_zero[8];  // zero this if you want to
		//                };
	struct sockaddr_in cliaddr, servaddr;

	//int socket(int domain, int type, int protocol);定义于<sys/socket.h>
	//参数说明：domain-可选项AF_UNIX,AF_INET
	//					AF_UNIX:文件路径
	//					AF_INET:网络地址
	//			type-可选项SOCK_STREAM,SOCK_DGRAM,SOCK_SEQPACKET
	//					SOCK_STREAM:可靠的
	//					SOCK_DGRAM:不可靠的
	//					SOCK_SEQPACKET:带记录的
	//			protocol-不知道干什么的，设定端口？
	//返回值：整形，还是nonnegative的，这个socket file的descriptor,不明.
	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
	//没看着哪里有解释这些东西都定义了啥。
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);//这里定义端口，上面有宏定义。

	//传说中的BLAB开始了。
	
	//传说中的端口绑定
	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	//确定队列长度	
	listen (listenfd, LISTENQ);
	
	printf("%s\n","Server running...waiting for connections.");

	for ( ; ; ) {
	
		clilen = sizeof(cliaddr);
		//传说中的blab的第三步，accept。
		connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
		printf("%s\n","Received request...");
		
		//blab第四步，accept，也就是开始通信了。
		while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
		//printf("%s","String received from and resent to the client:");
		//printf("%s",buf);
		puts(buf);
		send(connfd, "Send OK!\n", 15, 0);
		}
	
		if (n < 0) {
			perror("Read error");
			exit(1);
		}
		close(connfd);
	
	}
	//close listening socket
	close (listenfd);
}

