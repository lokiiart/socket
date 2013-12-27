#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "io.h"

//主要参考资料http://pubs.opengroup.org
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 5272 /*port*/
#define LISTENQ 8 /*maximum number of client connections */


//void error(char *msg)
//{
//		fprintf(stderr,"%s: %s\n", msg, strerror(errno));
//		exit(1);
//}

int open_listener_socket(){
		int s = socket (AF_INET, SOCK_STREAM, 0);
		if(s == -1)
				error("不能打开socket");
		return s;
}

void bind_to_port(int socket, int port)
{
		struct sockaddr_in servaddr;
		//没看着哪里有解释这些东西都定义了啥。
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(SERV_PORT);//这里定义端口，上面有宏定义。
		int reuse = 1;
		if(setsockopt(socket,SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)error("无法设置socket的重新使用端口选项");
		int c = bind (socket, (struct sockaddr *) &servaddr, sizeof(servaddr));
		if(c == -1)
		error("不能绑定端口");
}

//int say(int socket ,char *s)
//{
//		int result = send(socket, s, strlen(s), 0);
//		//出措时没有调用error(),因为error()会关闭服务端。
//		if(result == -1)
//				fprintf(stderr, "%s: %s\n", "通信时发生了错误",strerror(errno));
//		return result;
//}

int catch_singal(int sig, void (*handler)(int))
{
		struct sigaction action;
		action.sa_handler = handler;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		return sigaction (sig, &action, NULL);
}

int listener_d;

void handle_shutdown(int sig)
{
		if(listener_d)
				close(listener_d);
		fprintf(stderr, "再见!\n");
		exit(0);
}

void* do_std_in(void *a){
		char message[255];
		while(fgets(message,sizeof(message),stdin)){
				say(d_sock,message);
		}
		return NULL;

}

void* do_std_out(void *a)
{
		
		char rec[256];
		int bytesRcvd = recv(d_sock, rec, 255, 0 );
		while(bytesRcvd){
				if(bytesRcvd == -1)
						error("Can't read from server");
				rec[bytesRcvd] = '\0';
				printf("%s", rec);
				bytesRcvd = recv(d_sock, rec, 255, 0);
		}

		return NULL;

}

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
	struct sockaddr_in cliaddr;

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
//	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	listenfd = open_listener_socket();
	
	//没看着哪里有解释这些东西都定义了啥。
//	servaddr.sin_family = AF_INET;
//	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	servaddr.sin_port = htons(SERV_PORT);//这里定义端口，上面有宏定义。
	

	//传说中的BLAB开始了。
	
	//传说中的端口绑定
//	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	//确定队列长度	
	bind_to_port(listenfd, LISTENQ);
	listen (listenfd, LISTENQ);
	
	printf("%s\n","Server running...waiting for connections.");

	for ( ; ; ) {
	
		clilen = sizeof(cliaddr);
		//传说中的blab的第三步，accept。
		connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
		if(connfd == -1)
				error("不能打开第二个socket");
		printf("%s\n","Received request...");
		if(!fork()){
				close(listenfd);

				//To-do 在子进程中通信，向所有客户端发送所有说到的数据，向每个客户端发送相关的ip地址和socket信息，2.服务端可接收信息，并向客户端发送。3.可像单个客户端发送。
				//blab第四步，accept，也就是开始通信了。
				while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
					//printf("%s","String received from and resent to the client:");
					//printf("%s",buf); //puts(buf);
					//send(connfd, "Send OK!\n", 15, 0);
					puts(buf);
					say(connfd,"Send OK!\n");
					say(connfd,buf);
				}
			
				if (n < 0) {
					perror("Read error");
					exit(1);
				}
				close(connfd);
				exit(0);

				}
		
	
	}
	//close listening socket
	//close (listenfd);
	close(connfd);
}

