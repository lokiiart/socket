#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include "io.h"

void error(char *msg)
{
		fprintf(stderr, "%s: %s\n",msg, strerror(errno));
		exit(1);
}
int say(int socket ,char *s)
{
		int result = send(socket, s, strlen(s), 0);
		//出措时没有调用error(),因为error()会关闭服务端。
		if(result == -1)
				fprintf(stderr, "%s: %s\n", "通信时发生了错误",strerror(errno));
		return result;
}
