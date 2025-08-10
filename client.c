#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#define MAX_SIZE 1024

// typedef enum {
// 	PROTO_HELLO,
// } proto_type_e;

// typedef struct{

//     proto_type_e type;
//     unsigned short len;

// }proto_hdr_t;

// void handle_server(int fd){
//     char buf[4096] = {0};
//     proto_hdr_t *hdr = (proto_hdr_t*)&buf;

//     read(fd, hdr, sizeof(proto_hdr_t) + sizeof(int));

//     hdr->type = ntohl(hdr->type);
//     hdr->len = ntohs(hdr->len);
//     int *data = (int *)&hdr[1];

//     *data = ntohl(*data);


//     printf("type %d\n", hdr->type);
//     printf("len %d\n", hdr->len);
//     printf("data %d\n", *data);


//     if(hdr->type != PROTO_HELLO){
//         printf("No es el mateix protocol! \n");
//         return;
//     }

//     if(*data != 1 ){
//         printf("La versio del protocol es erronea!\n");
//         return;
//     }

//     printf("S'ha establit correctament la connexio!\n");

// }


// typedef enum {
// 	PROTO_HELLO,
// } proto_type_e;

// typedef struct{

//     proto_type_e type;
//     unsigned short len;

// }proto_hdr_t;


// void handle_client(int fd){

//     char buf[4096] = {0};
//     proto_hdr_t *hdr = (proto_hdr_t *)&buf;

//     hdr->type = htonl(PROTO_HELLO);
//     hdr->len = sizeof(int);
//     int reallen = hdr->len;
//     hdr->len = htons(hdr->len);

//     int *data = (int *)&hdr[1]; // No se que es aixo -.-

//     *data = htonl(1); // Versio del protocol v1. 

//     // for (int i = 1; i < sizeof(buf) / 2; i++) {
//     //     printf("%02x ", (unsigned char)buf[i]);
//     //     if(i%16 == 0){
//     //         printf("\n");
//     //     }
//     // }

//     write(fd, hdr, sizeof(proto_hdr_t) + reallen);


// }

typedef enum{

    TCP,
    COMM,

}proto_type_e; 


typedef struct{

    proto_type_e type;
    unsigned int len;
    
}proto_data_s; 

void checkServerConnection(int fd){

    char buff[MAX_SIZE] = {0};
    proto_data_s *protocol = (proto_data_s*)&buff;

    protocol->type = htons(TCP);
    protocol->len = htons(sizeof(proto_data_s) + sizeof(int));
    

    int *data = (int *)&protocol[1];

    *data = ntohl(1);

    send(fd, buff, sizeof(proto_data_s)+sizeof(int), MSG_NOSIGNAL);

}

typedef enum{
    serverChecked,
    serverNotChecked,
}server_status_e;

int main(){
    
    int fd, nfds; 
    struct sockaddr_in sockAddress;
    char buff[1024] = {0};
    fd = socket(AF_INET, SOCK_STREAM, 0);
    fd_set readfd, writefd;
    
    if(fd == EXIT_FAILURE){
        perror("socket");
        close(fd);
        return EXIT_FAILURE; 
    }

    sockAddress.sin_family = AF_INET;
    sockAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    sockAddress.sin_port = htons(9001); // 5555

    connect(fd, (struct sockaddr *)&sockAddress, sizeof(sockAddress));

    checkServerConnection(fd);
    nfds = fd + 1;
    while(1){
        FD_ZERO(&readfd);
        FD_ZERO(&writefd);

        FD_SET(0, &readfd);
        FD_SET(fd, &readfd);

        if(select(nfds,&readfd, &writefd, NULL,NULL) == -1){
            fprintf(stderr, "[ERROR] select: %s\n", strerror(errno));
            continue;
        }

        if(FD_ISSET(0, &readfd)){
            
            ssize_t len = read(0, buff, sizeof(buff) - 1);
            
            if(len > 0){
                buff[len] = '\0';
                send(fd, buff, sizeof(buff), MSG_NOSIGNAL);
                memset(buff,0,1024);
            }
            
        }
        
        if(FD_ISSET(fd, &readfd)){
            if(recv(fd, buff, 1024, MSG_NOSIGNAL) == 0){
                FD_CLR(fd, &readfd);
            }
            printf("socket: %s\n", buff);
            memset(buff,0,1024);
        }

        
    }
    close(fd);

    return 0;
}