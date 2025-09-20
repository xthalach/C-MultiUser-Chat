#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// #include "../include/functions.h"
#include "../include/common.h"

// TCP Protocol - A protocol to check if the user is using the legitim client aplicaction. 

// TLV - Type Length Value

typedef enum{

    TCP,
    COMM,

}proto_type_e; 


typedef struct{

    proto_type_e type;
    unsigned int len;
    
}proto_data_s; 

//  Falta posar if als sends amb el errno per veure on falla.
int checkClientConnection(int fd, user_data_s *user, int i){
    
    proto_data_s *protocol = (proto_data_s*)&user[i].buff;

    //recv(fd, protocol, MAX_BUFF_SIZE, SEND_FLAGS_DEFAULT);

    unsigned int type;
    unsigned int len;

    type = ntohl(protocol->type);
    len = ntohs(protocol->len);

    int *data = (int *)&protocol[1];
    int value = ntohl(*data);

    if(type == TCP){
        if(send(fd, "[!] Good Protocol Type\n", strlen("[!] Good Protocol Type\n"), MSG_MORE) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send type: %s\n",strerror(errno));
            return EXIT_FAILURE;
        }
    }else {
        
        if(send(fd, "[!] Wrong Protocol, Closing Connection\n", strlen("[!] Wrong Protocol, Closing Connection\n"), MSG_MORE) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send error type: %s\n",strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }

    if(len == sizeof(proto_data_s) + sizeof(int)){
        if(send(fd, "[!] Good Size\n", strlen("[!] Good Size\n"), MSG_MORE) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send len: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }else {
        if(send(fd, "[!] Wrong Size, Closing Connection\n", strlen("[!] Wrong Size, Closing Connection\n"), MSG_MORE) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send error len: %s\n", strerror(errno));

        }
        return EXIT_FAILURE;
    }

    if(value == 1){
        if(send(fd, "[!] Good Data Value\n", strlen("[!] Good Data Value\n"), MSG_MORE) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send value: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;       
    }else{
        if(send(fd, "[!] Wrong Data, Closing Connection\n", strlen("[!] Wrong Data, Closing Connection\n"), MSG_NOSIGNAL) == EXIT_ERROR){
            fprintf(stderr, "[ERROR] send error value: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;        
    }
    
}


void checkServerConnection(int fd){

    char buff[MAX_BUFF_SIZE] = {0};
    proto_data_s *protocol = (proto_data_s*)&buff;

    protocol->len = htons(sizeof(proto_data_s) + sizeof(int));
    protocol->type = htons(TCP);

    int *data = (int *)&protocol[1];

    *data = ntohs(1);

    send(fd, buff, sizeof(proto_data_s)+sizeof(int), SEND_FLAGS_DEFAULT);

}