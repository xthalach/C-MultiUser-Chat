// All functionalities of the server chat 
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

#include "../include/functions.h"
#include "../include/common.h"

int freeUser(user_data_s **users ,fd_set *readfd){
    for(int i = 0; i < MAX_USERS; i++){
        if((*users)[i].fd == -1){
            FD_SET((*users)[i].fd, readfd);
            return i;
        }
    }
    return -1;
}

void initialize(user_data_s **users){
    for(int i = 0; i < MAX_USERS; i++){
        (*users)[i].state = NEW;
        (*users)[i].fd = -1;
    }
    
}

void serverClose(user_data_s **users, fd_set *readfd){
    char closeMessage[19] = "[!] Server closed!\n";
    for(int i = 0; i < MAX_USERS; i++){
        if((*users)[i].fd != -1 && FD_ISSET((*users)[i].fd, readfd)){
            send((*users)[i].fd, closeMessage, strlen(closeMessage), SEND_FLAGS_DEFAULT);
        }
    }
}

void cleanUser(user_data_s **users, int fd){
    memset((*users)[fd].buff, 0, MAX_BUFF_SIZE);
    (*users)[fd].fd = -1;
    (*users)[fd].state = NEW;
}



void userRegister(){


}

void userSave(){

}



