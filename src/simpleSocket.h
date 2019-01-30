#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<ctype.h>

#define BUFFER_SIZE 1024 
//strcpy connot excess the BUFFER_SIZE 
struct SimpleChunk{
	int size, offset;
	char buffer[BUFFER_SIZE];
	bool endflag = false;
};


bool portVarify(const char* port);

void setTimeout(int sockfd, int send_time, int recv_time);

void simpleSocketSend(int sockfd, SimpleChunk* chunk, int chunk_size);

int simpleSocketRecv(int sockfd, SimpleChunk* chunk, int chunk_size);