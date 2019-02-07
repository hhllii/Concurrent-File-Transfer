#include"simpleSocket.h"

struct SimpleAddress getAddressbyLine(char* line){
    struct SimpleAddress simpleaddress;
    //read address
    char delim[]=" ";
    char* res = NULL;
    if((res=strtok(line,delim)) != NULL){
        simpleaddress.address = res;
        printf("*Address: %s\n",simpleaddress.address);
    }
    if((res=strtok(NULL,delim)) != NULL && portVarify(res)){
        simpleaddress.port = atoi(res);
        printf("*Port: %i\n",simpleaddress.port);
    }else{ //error port set to default port num
        simpleaddress.port = DEFAULT_PORT;
    }
    return simpleaddress;
}

bool portVarify(const char* port){
    //-1 for fgets() \n
    for(int i = 0; i < strlen(port) - 1; ++i){
        if(!isdigit(port[i])){
            //inclued non-digit
            return false;
        }
    }
    if(atoi(port) < 0 || atoi(port) > 65535){
        //invalid number
        return false;
    }
    return true;
}


void simpleSocketSend(int sockfd, SimpleChunk* chunk, int chunk_size){
    // Send data
    if (send(sockfd, chunk, chunk_size, 0) < 0)
    {
        printf("Send data error: %s(errno: %d)\n", strerror(errno), errno);
    }
    printf("*Sending: \n%s\n", chunk->buffer);
}

int simpleSocketRecv(int sockfd, SimpleChunk* chunk, int chunk_size){
        int recv_size = (int)recv(sockfd, chunk, chunk_size, 0);
        if( recv_size >0 )
        {
            // Handle the buffer
            // if(chunk->endflag){
            //     return 0;
            // }else{
            //     return 1;
            // }
            //printf("*Received message: \n%s\n",buffer );
        }
        else{
            // Handle socket recv error
            if((recv_size<0) &&(recv_size == EAGAIN||recv_size == EWOULDBLOCK||recv_size == EINTR)) //error code, connection doesn't fail continue
            {
                printf("\n Socket error %s(errno: %d)\n", strerror(errno),errno);
                return -1;
            }
            //printf("*End of receive \n");
            return 0;
        }
}

void setTimeout(int sockfd, int send_time, int recv_time){
    struct timeval timeout_send={send_time,0};//3s
    struct timeval timeout_recv={recv_time,0};//3s

    int ret_send=setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout_send,sizeof(timeout_send));
    int ret_recv=setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout_recv,sizeof(timeout_recv));

}