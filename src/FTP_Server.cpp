#include "FTP_Server.h"

int main(int argc, char** argv){
    int  listenfd, connfd;
    struct sockaddr_in  servaddr;
    //char  buffer[1024];
    int  n;
    SimpleChunk chunk;
    SimpleChunk *chunkPtr = &chunk;
    char* filename;

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
                printf("Received filename from client: %s\n", chunk.buffer);
                printf("Num-connection: %d\nOffset: %d\n", chunk.size, chunk.offset);
                filename = chunk.buffer;
            }


            char filePath[MAX_PATH_LEN];
            strcpy(filePath,FILE_PATH);
            strcat(filePath,filename);
            // Handle the command
            FILE *fp;
            //const char* filePath = "set"; // setfile
            fp = fopen(filePath, "r");
            if(fp == NULL){
                printf("open file error!\n");
                // fclose(fp);
                continue;
            }

            // Get file size
            int fileSize = getFileSize(fp);
            printf("*File size: %i\n", fileSize);

            // Get the file part size for each thread
            int partSize = fileSize/chunk.size; //*bug might have some precise lose with the last part of file
            printf("*File part size: %i\n", partSize);

            // Send file
            char fileBuffer[1000];
            int block_len = 0;
            struct SimpleChunk sendchunk;
            struct SimpleChunk *sendchunkPtr = &sendchunk;
            int sendCount = 0;
            // Fp to send start pos
            fseek(fp, chunk.offset * partSize, SEEK_SET );

            while ((sendCount + 1000) < partSize && (block_len = fread(fileBuffer, sizeof(char), 1000, fp)) > 0 )
            {
                // Send data
                //clean struct
                memset(&sendchunk,0,sizeof(sendchunk));
                sendchunk.size = block_len;
                strcpy(sendchunk.buffer, fileBuffer);
                simpleSocketSend(connfd, sendchunkPtr, sizeof(struct SimpleChunk));
                memset(fileBuffer,0,strlen(fileBuffer));
                //sleep(2); //test for sending block
                sendCount += block_len;
            }
            //send last block
            if(chunk.offset == chunk.size - 1){
                //last part of fiile
                memset(&sendchunk,0,sizeof(sendchunk));
                int lastLen = fileSize - chunk.offset * partSize;
                block_len = fread(fileBuffer, sizeof(char), lastLen, fp); 
                sendchunk.size = block_len;
                strncpy(sendchunk.buffer, fileBuffer, block_len);
                //strcpy(sendchunk.buffer, fileBuffer);
                simpleSocketSend(connfd, sendchunkPtr, sizeof(struct SimpleChunk));
                memset(fileBuffer,0,strlen(fileBuffer));
            }else{//send remaining byte
                memset(&sendchunk,0,sizeof(sendchunk));
                block_len = fread(fileBuffer, sizeof(char), partSize - sendCount, fp); 
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