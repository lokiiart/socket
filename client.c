#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include "io.h"
int d_sock;

//void error(char *msg)
//{
//		fprintf(stderr, "%s: %s\n",msg, strerror(errno));
//		exit(1);
//}
//
int open_socket(char *host, char *port)
{
		struct addrinfo *res;
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		if(getaddrinfo(host, port, &hints, &res) == -1)
				error("Can't resolve the address");
		int d_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (d_sock == -1)
				error("Can't open socket");
		int c = connect(d_sock, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);
		if(c == -1)
				error("Can't connect to socket");
		return d_sock;
}

//int say(int socket, char *s)
//{
//		int result = send(socket, s, strlen(s), 0);
//		if(result == -1)
//				fprintf(stderr, "%s: %s\n","Error talking to server",strerror(errno));
//		return result;
//
//}

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

int main(int argc, char *argv[])
{

		d_sock = open_socket("localhost","5272");
		pthread_t t0;
		pthread_t t1;
		if(pthread_create(&t0, NULL, do_std_in, NULL) == -1)
				error("无法创建线程t1");
		if(pthread_create(&t1, NULL, do_std_out,  NULL) == -1)
				error("无法创建线程t2");

		void * result;
		if(pthread_join(t0, &result) == -1)
				error("Can't gather t1");
		if(pthread_join(t1, &result) == -1)
				error("Can't gather t2");

		//sprintf(buf, "GET / http:/1.1\r\n");
		//say(d_sock,buf);
		//say(d_sock, "Host: localhost\r\n\r\n");
		close(d_sock);
		return 0;
}
