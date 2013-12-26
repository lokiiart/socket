#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "io.h"

void* do_std_in(void *a)
{
		char message[255];
		while(fgets(message,sizeof(message),stdin)){
				printf("Get from input:%s",message);
		}
		return NULL;
}

void * do_std_out(void *a)
{

		int i = 0;

		for(i = 0; i< 100; i++){
				sleep(1);
				//printf("不支持中文");
				puts("另一线程正在计数：");
				printf("%d\n",i);
		}
		return NULL;
}

int main(int argc, char *argv[])
{
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
		return 0;
}

