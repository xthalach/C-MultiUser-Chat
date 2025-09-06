// All functionalities of the server chat 
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

#include "../include/functions.h"
#include "../include/common.h"

int freeUser(user_data_s *users ,fd_set *readfd){
    for(int i = 0; i < MAX_USERS; i++){
        if(strcmp(users[i].username, "") == 0){
            FD_SET(users[i].fd, readfd);
            return i;
        }
    }
    return -1;
}

void initialize(user_data_s *users){
    for(int i = 0; i < MAX_USERS; i++){
        users[i].state = PROTOTEST;
        users[i].authState = AUTHNONE;
        //strcpy(users[i].username, "");
        users[i].id = i;
        users[i].fd = -1;
    }
    
}

// aixo no se si funciona. 
void serverClose(user_data_s *users, fd_set *readfd){
    char closeMessage[19] = "[!] Server closed!\n";
    for(int i = 0; i < MAX_USERS; i++){
        if(users[i].fd != -1 && FD_ISSET(users[i].fd, readfd)){
            send(users[i].fd, closeMessage, strlen(closeMessage), SEND_FLAGS_DEFAULT);
        }
    }
}

void cleanUser(user_data_s *users, int fd){
    memset(users[fd].buff, 0, MAX_BUFF_SIZE);
    users[fd].fd = -1;
    users[fd].state = NEW;
}

void listUsers(user_data_s *users, int maxUsers){
    
    for(int i = 0; i < maxUsers; i++){
        printf("ID %d\n", users[i].id);
        printf("username: %s\n", users[i].username);
        printf("password: %s\n", users[i].password);
        printf("rol: %s\n", users[i].rol);
        printf("fd: %d\n", users[i].fd);
        printf("buff: %s\n", users[i].buff);
        printf("currentChat: %d\n", users[i].currentChat);
        printf("state: %d\n", users[i].state);
        printf("menuState: %d\n", users[i].menuState);
        printf("authState: %d\n\n", users[i].authState);
    }

}