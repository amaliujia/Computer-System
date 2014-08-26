#include "include.h"
#include "cache.h"
#include "sem.h"
/*
 * My function declarations end
 */
#define PMD_MAX_SIGNALS 64

void *thread_main(void *argv);
void handler(int signum);
//void signalInit();

int main(int argc, char *argv[])
{
	int listenfd, port, clientLen = 1;
	struct sockaddr_in clientaddr;	
	pthread_t tid;
	if(argc != 2){
		unix_error("The number of paremeters is not enough\n");
	}
 	signal(SIGPIPE, SIG_IGN);
	cacheInit(&cache);
	port = atoi(argv[1]);	
	listenfd = Open_listenfd(port);
	while(1){
		memset(&clientaddr, 0, sizeof(struct sockaddr_in));
		clientLen = sizeof(clientaddr);
		int temp;
		temp = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientLen);
		int *condfd = malloc(sizeof(int));
		*condfd = temp;
		Pthread_create(&tid, NULL, thread_main, condfd);
	}
	Close(listenfd);
	cacheDestroy(&cache);	
    return 0;
}

// Entry to thread
void *thread_main(void *argv){
	int fd = *(int *)argv;
	pthread_detach(pthread_self());
	free(argv);
	afterAccept(fd);
	Close(fd);
	return NULL;
}
