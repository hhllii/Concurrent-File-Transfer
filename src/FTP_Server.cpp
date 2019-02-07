#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<ctype.h>
#include "simpleSocket.h"


int main(int argc, char** argv){
    int  listenfd, connfd;
    struct sockaddr_in  servaddr;
    //char  buffer[1024];
    int  n;
    SimpleChunk chunk;
    SimpleChunk *chunkPtr = &chunk;
    char* filePath;

    if(argc != 2){
        printf("Usage: ./myserver <port>\n");
        return 0;
    }

    // Varify port
    if(!portVarify(argv[1])){
        printf("\n Port invalid\n"); 
    }
    // Listen socket create
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("Create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    int port_num = atoi(argv[1]);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port_num);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("Bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    //listenning
    if( listen(listenfd, 10) == -1){
        printf("Listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    printf("======Waiting for client's request======\n");
    while(1){
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("Accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }else{
            // Set timeout
            setTimeout(connfd, 3, 0);
            //connection success receiving file name
            memset(&chunk,0,sizeof(struct SimpleChunk));
            if(simpleSocketRecv(connfd, chunkPtr, sizeof(struct SimpleChunk)) < 0){
                //error with socket
                printf("*End of client \n");
                continue;
            }else{
                printf("Received message from client: %s\n", chunk.buffer);
                filePath = chunk.buffer;
            }

            // Handle the command
            FILE *fp;
            //const char* filePath = "set"; // setfile
            fp = fopen(filePath, "r");
            if(fp == NULL){
                printf("open file error!\n");
                fclose(fp);
                continue;
            }

            // Get file size
            int file_size = getFileSize(fp);
            printf("*File size: %i\n", file_size);

            // Send file
            char fileBuffer[1000];
            int block_len = 0;
            SimpleChunk sendchunk;
            SimpleChunk *sendchunkPtr = &sendchunk;
            while( (block_len = fread(fileBuffer, sizeof(char), 1000, fp)) > 0)
            {
                // Send data
                //clean struct
                memset(&sendchunk,0,sizeof(sendchunk));
                sendchunk.size = block_len;
                strcpy(sendchunk.buffer, fileBuffer);
                simpleSocketSend(connfd, sendchunkPtr, sizeof(struct SimpleChunk));
                memset(fileBuffer,0,strlen(fileBuffer));
            }
            printf("*End of data send\n");
            //free(sendchunkPtr);
            // Close file 
            if(fclose(fp) == -1){
                printf("Close file error!\n");
                exit(1);
            }
        }
        // End of this connection
        close(connfd);
    }
    close(listenfd);
    return 0;
}