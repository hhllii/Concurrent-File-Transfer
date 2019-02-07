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
#include <pthread.h>


#include "simpleSocket.h"

using namespace std;

// void *downloadThread(void){

// }

int main(int argc, char const *argv[]) 
{ 
    int sockfd = 0;
    struct sockaddr_in serv_addr; 
    int connectNum;
    if(argc != 4){
        printf("usage: ./myclient <server-info.txt> <num-connections> <filename>\n");
        return 0;
    }
    // Handle the num-connections
    if(!checkdigit(argv[2])){
        printf("Invalid num-connections\n");
        return 0;
    }
    if(atoi(argv[2]) > MAX_SERVER){
        printf("Excess the max num-connections\n");
        return 0;
    }

    const char* serverFilename = argv[1]; //set servers list
    connectNum = atoi(argv[2]); //set connection num
    const char* filename = argv[3]; //set file name
    FILE *serverFp = fopen(serverFilename, "r");
    if(serverFp == NULL){
        printf("open server-info file error! No such file: %s\n", argv[1]);
        fclose(serverFp);
        exit(1);
    }

    // Read address list
    //struct SimpleAddress addList[MAX_SERVER];
    vector<SimpleAddress> addList;
    char serverBuffer[1024];
    int add_index = 0;
    while(fgets(serverBuffer, 1024, serverFp) != NULL){
        printf("*address line: %s\n", serverBuffer);
        addList.push_back(getAddressbyLine(serverBuffer));
    }

    //add_index - 1 is the last address

    int port = addList[0].port;
    char* address = addList[0].address;

    fclose(serverFp);

    vector<int> socklist = getActiveSockList(addList);
    for(auto sock : socklist){
        printf("socket id: %i \n", sock);
    }

//==================================Thread Start==========================================

    // Create socket
    sockfd = socklist[0];

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
            //printf("%s", recvchunk.buffer);
            fwrite(recvchunk.buffer, sizeof(char), strlen(recvchunk.buffer), fp);
        }else{
            //printf("%s\n", recvchunk.buffer);
            printf("*End of data recv\n");
            break;
        }
    }
    //free(chunkPtr);
    // End of connection
    close(sockfd);
    return 0; 
} 