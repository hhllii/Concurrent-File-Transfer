#include "FTP_Client.h"

using namespace std;

void *downloadThread(void *arg){
    struct ThreadAttri *temp;
    temp = (struct ThreadAttri *)arg;
    // Get Attri
    int connectNum = temp -> connectNum;
    int *socklist = temp -> socklist;
    int t_idx = temp -> t_idx;
    // Create socket
    int sockfd = socklist[t_idx];
    printf("Thread %d start\n", t_idx);
    // Send filename
    char* filename = temp -> filename; //only the ori filename will be modified as create new file
    struct SimpleChunk chunk;
    struct SimpleChunk* chunkPtr = &chunk;
    strcpy(chunk.buffer, filename); //*bug filename might be longer than 1024
    chunk.endflag = true;
    simpleSocketSend(sockfd, chunkPtr, sizeof(struct SimpleChunk));
    printf("*Client filename sent %s\n", chunk.buffer); 

    // Recv file and store
    printf("*Server return message: \n");
    SimpleChunk recvchunk;
    SimpleChunk *recvchunkPtr = &recvchunk;
    // Construct file path
    char filepath[MAX_PATH_LEN]; //*bug didn't check the len if filename too long 
    char filepre[5];
    sprintf(filepre,"%d", t_idx); 
    strcpy(filepath,"./dest/");
    strcat(filepath,filepre);
    strcat(filepath,"_");
    strcat(filepath,filename);
    FILE* fp = fopen(filepath, "w");
    if(fp == NULL){
        printf("Create file error!\n");
        pthread_exit((void*)-1);
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
    // End of connection
    //close(sockfd);
    fclose(fp);
    pthread_exit((void*)1);
}


//start client
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
        exit(1);
    }

    // Read address list
    //struct SimpleAddress addList[MAX_SERVER];
    vector<SimpleAddress> addList;
    char serverBuffer[1024];
    int add_index = 0;
    //add all address
    while(fgets(serverBuffer, 1024, serverFp) != NULL){
        struct SimpleAddress sa;
        printf("*address line: %s\n", serverBuffer);
        addList.push_back(getAddressbyLine(serverBuffer));
    }
    //add_index - 1 is the last address

    int port = addList[0].port;
    char* address = addList[0].address;

    fclose(serverFp);

    vector<int> vsocklist = getActiveSockList(addList);

    //set connection num
    int activeNum = vsocklist.size();
    connectNum = min(connectNum, activeNum);

    //vector to int []
    int socklist[MAX_SERVER];
    for(int i =0; i < vsocklist.size(); ++i){
        socklist[i] = vsocklist[i];
        printf("socket id: %i \n", socklist[i]);
    }


//==================================Thread Start==========================================
    //create thread
    vector<pthread_t> threadList;
    vector<struct ThreadAttri*> attrilist;
    struct ThreadAttri *ptAttri;
    //struct ThreadAttri *ptAttri[connectNum];
    for(int i = 0; i < connectNum; ++i){//i >= number of server
        pthread_t thid;
        ptAttri = (struct ThreadAttri*)malloc(sizeof(struct ThreadAttri));
        attrilist.push_back(ptAttri);
        ptAttri->connectNum = connectNum;
        memcpy(ptAttri->socklist,socklist,sizeof(socklist));
        // tAttri->socklist = socklist;
        ptAttri->t_idx = i;
        memcpy(ptAttri->filename,filename,sizeof(filename));
        // tAttri->filename = filename;
        if (pthread_create(&thid,NULL,downloadThread,(void*)ptAttri) == -1){
             printf("Thread create error!\n");
             return -1;
        }
        threadList.push_back(thid);
    }
    // Join thread
    for(int i = 0; i < connectNum; ++i){
        // int *t_res;
        // if (pthread_join(threadList[i], (void**)&t_res)){
        if (pthread_join(threadList[i], NULL)){ 
            printf("Thread is not exit...\n");
            return -1;
        } 
        // if(*t_res == -1){ 
        //     printf("Thread failed to download file\n");
        //     return 0;
        // }
        free(attrilist[i]);// Free attri
    }
    



    // // Create socket
    // sockfd = socklist[0];

    // // Send filename
    // SimpleChunk chunk;
    // SimpleChunk* chunkPtr = &chunk;
    // strcpy(chunk.buffer, filename); //*bug filename might be longer than 1024
    // chunk.endflag = true;
    // simpleSocketSend(sockfd, chunkPtr, sizeof(struct SimpleChunk));
    // printf("*Client filename sent\n"); 

    // printf("*Server return message: \n");
    // SimpleChunk recvchunk;
    // SimpleChunk *recvchunkPtr = &recvchunk;
    // FILE* fp = fopen("cset", "w");
    // if(fp == NULL){
    //     printf("Create file error! No such file: %s\n", argv[1]);
    //     fclose(fp);
    //     exit(1);
    // }
    // while(1){
    //     memset(&recvchunk,0,sizeof(recvchunk));
    //     // memset(recvchunk.buffer,'0',strlen(recvchunk.buffer));
    //     if(simpleSocketRecv(sockfd, recvchunkPtr, sizeof(struct SimpleChunk)) > 0){
    //         printf("%s", recvchunk.buffer);
    //         fwrite(recvchunk.buffer, sizeof(char), strlen(recvchunk.buffer), fp);
    //     }else{
    //         printf("%s\n", recvchunk.buffer);
    //         printf("*End of data recv\n");
    //         break;
    //     }
    // }
    //free(chunkPtr);
    // End of connection
    //close all socket
    for(int i = 0; i < connectNum; ++i){
        close(socklist[i]);
    }
    
    return 0; 
} 