#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/select.h>

#include <errno.h>

#include "../include/functions.h"
#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/database.h"
#include "../include/database.h"

int main(){

    int sockfd, nfds, clifd; 
    struct sockaddr_in sockAddress = {0}, clientAddress = {0};
    socklen_t clientAddressLen = sizeof(clientAddress);

    user_data_s *users;
    users = (user_data_s *)calloc(MAX_USERS, sizeof(user_data_s));


    fd_set readfd;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == EXIT_FAILURE){
        perror("socket");
        close(sockfd);
        return EXIT_FAILURE; 
    }

    sockAddress.sin_family = AF_INET;
    //sockAddress.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    sockAddress.sin_addr.s_addr = INADDR_LOOPBACK;
    sockAddress.sin_port = htons(9001); // 5555

    if(bind(sockfd, (struct sockaddr *)&sockAddress, sizeof(sockAddress)) == -1){
        perror("Bind\n");
        close(sockfd);
        return 1;
    }
   

    if(listen(sockfd, BACKLOG) == -1){
        perror("Listen\n");
        close(sockfd); 
        return -1;
    }
    
    nfds = sockfd + 1;

    initialize(&users);
    
    while(1){
        FD_ZERO(&readfd);
        FD_SET(sockfd, &readfd);

        for(int i = 0; i < MAX_USERS; i++){
            if(users[i].fd != -1){
                FD_SET(users[i].fd, &readfd);
            }
        }

        // // Reset the nfds variable to reuse the log off file descriptors from clients. This could make that another user gets a used file descriptor
        // for(int i = 0; i < MAX_USERS; i++){
        //     if(users[i].fd != -1){
        //         if(nfds >= users[i].fd) nfds = users[i].fd + 1;
        //     }
        // }

        if(select(nfds, &readfd, NULL, NULL, NULL) == -1){
            fprintf(stderr, "[ERROR] SELECT: %s\n", strerror(errno));
            continue;
        }


        if(FD_ISSET(sockfd, &readfd)){
            if((clifd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1){
                perror("Accept\n");
                serverClose(&users, &readfd);
                close(sockfd);
                return -1;
            }else{
                
                int user;
                if((user = freeUser(&users ,&readfd)) == -1){
                    char usersFull[41] = "[!] Not enoght space the users are full!\n";
                    send(clifd, usersFull, strlen(usersFull), SEND_FLAGS_DEFAULT);
                    continue;
                }
                users[user].fd = clifd;
                users[user].state = PROTOTEST;
                strcpy(users[user].username,"Testing\0");
                if(nfds <= clifd) nfds = clifd + 1;

                //handle_client(users[user].fd);
                // if(checkClientConnection(users[user].fd) == -1){
                //     close(users[user].fd);                    
                //     users[user].fd = -1;

                // }
            }
        }


        for(int i = 0; i < MAX_USERS; i++){
            if(users[i].fd != -1 && FD_ISSET(users[i].fd, &readfd)){
                if(recv(users[i].fd,users[i].buff,MAX_BUFF_SIZE, SEND_FLAGS_DEFAULT) <= 0){
                    saveUsersData(users);
                    cleanUser(&users, i);                   
                    continue;
                }else{
                    if(users[i].state == PROTOTEST){
                        // Chapusa
                        if(checkClientConnection(users[i].fd, users, i) == 1){
                            close(users[i].fd);
                            users[i].fd = -1;
                        }else{
                            // Login //
                            // Welcome to the C multichat server
                            // [1] Login
                            // [2] Register
                            // [3] Forgot Account
                            users[i].state = CONNECTED;
                        }
                    }else{
                        if(send(users[i].currentChat, users[i].buff, MAX_BUFF_SIZE, SEND_FLAGS_DEFAULT) == -1){
                            fprintf(stderr, "[ERROR] SEND currentChat: %s\n", strerror(errno));
                        }
                        memset(users[i].buff, 0, MAX_BUFF_SIZE); // Clean the user buffer.                         
                    }

                }
            }
        }
        

    }

    close(sockfd);

    return 0;
}
