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

#define DEST_PATH "./dest/"

struct ThreadAttri{
    int t_idx, connectNum;
    char filename[MAX_FILENAME_LEN]; //use one buffer 
	int socklist[MAX_SERVER]; 
};
