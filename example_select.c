#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>

#define BACKLOG 5
#define MAX_CLIENT 256
#define BUF_SIZE 4096

#define WELCOME "Benvingut al servidor\n"
#define GOODBYE "Hasta la proxima!\n"

// Definicio del protocol 

typedef enum {
	PROTO_HELLO,
} proto_type_e;

typedef struct{

    proto_type_e type;
    unsigned short len;

}proto_hdr_t;

// TLV = Type Lenght Value
void handle_header_clients(int fd){
    char buff[1024] = {0};
    proto_hdr_t *hdr = (proto_hdr_t*)&buff;

    hdr->type = htonl(PROTO_HELLO);
    int reallen = sizeof(proto_hdr_t) + sizeof(int);
    hdr->len = htons(reallen);

    
    int *data = (int*)&hdr[1];
    int *data2 = (int*)((char*)&buff + sizeof(proto_hdr_t));

    printf("data: %p\n", data);
    printf("data2: %p\n",data2);

    *data = ntohl(1);

    write(fd, hdr, reallen);
    

}



typedef enum{
    NEW,
    CONNECTED,
    DISCONECTED
}connstate_t;

// Estructura dels clients 
typedef struct{
    int fd;
    connstate_t state;
    char buf[4096];
} clientdata_t;

clientdata_t clients[MAX_CLIENT] = {0}; 

void initialize(){
    for(int i = 0; i < MAX_CLIENT; i++){

        clients[i].fd = -1; // Per saber que el client esta lliure
        clients[i].state = NEW; 
        memset(&clients[i].buf, 0, sizeof(BUF_SIZE)); 
    
    }

}

int isFree(){

    for(int i = 0; i < MAX_CLIENT; i++){
        //printf("isFree() FDClient%d: %d\n", i, clients[i].fd);
        if(clients[i].fd == -1){
            return i;
        }; // Per saber que el client esta lliure
          
    }
    // Estan tots els clients ocupats.
    return EXIT_FAILURE;
}




int main(){

    int listen_fd, nfds, conn, cli; 
    // 00000000000000000000000
    fd_set readfds, writefds;
    struct sockaddr_in sockAddress = {0}, sockClient = {0};
    socklen_t clientAddrLen = sizeof(struct sockaddr_in); 

    // Inicialitzem els clients; 
    initialize();

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE; 
    }

    sockAddress.sin_family = AF_INET; 
    sockAddress.sin_addr.s_addr = IPPROTO_IP;
    sockAddress.sin_port = htons(9001); 

    printf("%d\n",listen_fd);

    if(bind(listen_fd, (struct sockaddr *)&sockAddress, sizeof(struct sockaddr_in)) == -1){
        perror("bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }


    if(listen(listen_fd, BACKLOG) == -1){
        perror("listen");
        close(listen_fd);
        return EXIT_FAILURE;

    }

    // nfds should be 1 more than the highest file descriptor number
    nfds = listen_fd + 1;
    
    while(1){
        // Initialize fd1 and fd2 with valid socket file descriptors
        // 00000000000000000000
        // 00000100101001000000
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);          
        FD_SET(listen_fd, &readfds);        
        
        // Es posible que el files descriptors no siguen consecutius? 3,4,5,8,9
        
        for(int i=0;i < MAX_CLIENT; i++){
            //printf("FDClient%d: %d\n", i, clients[i].fd);
            if(clients[i].fd != -1){
                FD_SET(clients[i].fd, &readfds);
                FD_SET(clients[i].fd, &writefds);
                if(nfds <= clients[i].fd) nfds = clients[i].fd + 1;
            }
        }



        if (select(nfds, &readfds, &writefds, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(listen_fd, &readfds)) {
            if((conn = accept(listen_fd, (struct sockaddr *)&sockClient, &clientAddrLen)) == -1 ){
                perror("accept");
                continue;
            }
            // Necesitem una funcio per saber si hi ha algun client disponible i que torne el valor del client lliure
            if((cli = isFree()) == -1){
                printf("Tots els clients estan ocupats!\n");
                close(listen_fd);
                return EXIT_FAILURE;
            }else {
                //printf("Cli: %d \n", cli);
                clients[cli].fd = conn;
                clients[cli].state = CONNECTED;
                handle_header_clients(clients[cli].fd);

                if(write(clients[cli].fd, WELCOME, sizeof(WELCOME)) == -1){
                    perror("write");
                    close(clients[cli].fd);
                    continue;
                } 
            }
            
        }
        // Al tanacar la sessio de una de les conexions estanquen totes. 

        for(int i=0; i < MAX_CLIENT; i++){
            if(clients[i].fd != -1 && FD_ISSET(clients[i].fd, &readfds)){
                printf("FD: %d\n", clients[i].fd);
                if(read(clients[i].fd, &clients[i].buf, BUF_SIZE) <= 0){
                    perror("read");
                    clients[i].fd = -1;
                    clients[i].state = DISCONECTED;
                    close(clients[i].fd);
                    
                }else{
                    printf("%s\n", clients[i].buf);
                    memset(&clients[i].buf, 0 , BUF_SIZE);
                }
                
            }
        }


    }
    close(listen_fd);
    return 0;
}