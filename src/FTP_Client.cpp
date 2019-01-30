#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <string> 
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include "simpleSocket.h"

int main(int argc, char const *argv[]) 
{ 
    int sockfd = 0;
    struct sockaddr_in serv_addr; 
    if(argc != 4){
        printf("usage: ./myclient <server-info.txt> <num-connections> <filename>\n");
        return 0;
    }
    const char* filename = argv[3];
    const char* serverFilename = "server-info.txt";
    // FILE *serverFp = fopen(filename, "r"));
    // if((serverFp == NULL){
    //     printf("open server-info file error! No such file: %s\n", argv[1]);
    //     fclose(serverFp);
    //     exit(1);
    // }

    //read address
    // char* serverBuffer;
    // fgets(serverBuffer, 1024, serverFp);
    // const char* delim=" ";
    // char* p=strtok(serverBuffer,delim);
    // char* address;
    // strcpy(address, p);
    // printf("*Address: %s\n",address);
    // p=strtok(NULL,delim);
    // char* port;
    // strcpy(port, p);
    // printf("*Port: %s\n",port);
    const char* address = "127.0.0.1";
    const char* port = "12345";

    // fclose(serverFp);
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Socket creation error %s(errno: %d)\n", strerror(errno),errno);
        return -1; 
    }else{
        printf("Socket created \n"); 
    }
    // Create socket address
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    int port_num = atoi(port);
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port_num); 

    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)  
    { 
        printf("\n Invalid address: %s\n",address);
        return -1; 
    } 
    // Socket connection
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\n Connection Failed %s(errno: %d)\n",strerror(errno),errno);
        return -1; 
    } 
    // Set timeout
    setTimeout(sockfd, 3, 3);

    // Send filename
    SimpleChunk chunk;
    SimpleChunk* chunkPtr = &chunk;
    strcpy(chunk.buffer, filename); //*bug filename might be too long than 1024
    chunk.endflag = true;
    simpleSocketSend(sockfd, chunkPtr, sizeof(struct SimpleChunk));
    printf("*Client filename sent\n"); 

    printf("*Server return message: \n");
    SimpleChunk recvchunk;
    SimpleChunk *recvchunkPtr = &recvchunk;
    FILE* fp = fopen("cset", "w");
    if(fp == NULL){
        printf("open server-info file error! No such file: %s\n", argv[1]);
        fclose(fp);
        exit(1);
    }
    while(1){
        memset(&recvchunk,0,sizeof(recvchunk));
        // memset(recvchunk.buffer,'0',strlen(recvchunk.buffer));
        if(simpleSocketRecv(sockfd, recvchunkPtr, sizeof(struct SimpleChunk)) > 0){
            printf("%s", recvchunk.buffer);
            fwrite(recvchunk.buffer, sizeof(char), strlen(recvchunk.buffer), fp);
        }else{
            //printf("%s\n", recvchunk.buffer);
            printf("*End of data \n");
            break;
        }
    }
    //free(chunkPtr);
    // End of connection
    close(sockfd);
    return 0; 
} 