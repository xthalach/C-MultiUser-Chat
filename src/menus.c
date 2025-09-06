#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/common.h"
#include "../include/database.h"



//  ****************************************************
//  *                                                  *
//  *  FUNCIONS           MENU               LOGIN     *
//  *                                                  *
//  ****************************************************

int login(db_header_s *db_header,user_data_s *users, int user, int clifd, char *tmpUser, char *tmpPass){
    users[user].menuState = LOGIN;

    if(users[user].authState == AUTHNONE){
        send(users[user].fd, "[+] Username: \n", 15,MSG_NOSIGNAL);
        users[user].authState = USER;
    }else if(users[user].authState == USER){
        strcpy(tmpUser, users[user].buff);
        send(users[user].fd, "[+] Password: \n", 15,MSG_NOSIGNAL);
        users[user].authState = PASSWORD;
    }else if(users[user].authState == PASSWORD){

        strcpy(tmpPass, users[user].buff);
        
        for(int i = 0; i < db_header->usersLen; i++){
            printf("user: %s\n",tmpUser);
            if(strcmp(users[i].username, tmpUser) == 0){
                printf("username ok\n");
                if(strcmp(users[i].password, tmpPass) == 0){
                    printf("password ok\n");
                    users[i].fd = clifd;
                    users[i].menuState = MENUNONE;
                    users[i].authState = AUTHNONE; 
                    users[i].state = AUTHENTICATED;
                    if(user != i) {
                        users[user].fd = -1;
                    }
                    memset(users[i].buff, 0, MAX_BUFF_SIZE); // Llimpesa de buffer best practice.. 
                    strcpy(tmpUser, "");
                    strcpy(tmpPass, "");
                    send(users[i].fd, "[+] Authenticated [+]\n", 1024, MSG_NOSIGNAL);
                    send(users[i].fd, "[+] USER MENU\n[1] List Online Users\n[2] Edit Username\n[3] Edit Password\n[4] Select User Chat\n[5] Show Waiting Room\n[6] Delete Account\n[7] Log Off\n", 194, MSG_NOSIGNAL);
                    return 1;
                }
            }
        }
        return -1;
    }
}

void newUserRegister(user_data_s *users, int i){
    users[i].menuState = REGISTER;

    if(users[i].authState != USER && users[i].authState != PASSWORD){
        send(users[i].fd, "[+] Username: \n", 15,MSG_NOSIGNAL);
        users[i].authState = USER;
        strcpy(users[i].rol, "USER");
    }else if(users[i].authState == USER){
        strcpy(users[i].username, users[i].buff);

        send(users[i].fd, "[+] Password: \n", 15,MSG_NOSIGNAL);
        users[i].authState = PASSWORD;
    }else if(users[i].authState == PASSWORD){

        strcpy(users[i].password, users[i].buff);
        send(users[i].fd, "[+] Account Created! \n\nLogin\nWelcome to the C multichat server\n[1] Login\n[2] Register\n[3] Forgot Account\n[4] Exit\n", 122,MSG_NOSIGNAL);
        //send(users[i].fd, users[i].password, PASSWORD_SIZE, MSG_NOSIGNAL);
        // S'han de posar per defecte les variables  
        users[i].menuState = MENUNONE;
        users[i].authState = AUTHNONE; 
        users[i].state = MENU;
    }
    
    //send(users[i].fd, "Inside register funtion\n", 25, MSG_NOSIGNAL);
}

void exitChat(user_data_s *users, int i){
    send(users[i].fd, "[+] See you next time [+]\n", 26, MSG_NOSIGNAL);
    close(users[i].fd);
    users[i].fd = -1;
    memset(users[i].buff, 0, MAX_BUFF_SIZE);
    users[i].currentChat = 0; // Que pasa si el FD es 0 i el usuari parla? Sortira el misatge per el terminal? 
    users[i].state = DISCONECTED; 
    users[i].menuState = MENUNONE;
    users[i].authState = AUTHNONE;
    
}


//  ****************************************************
//  *                                                  *
//  *  FUNCIONS           MENU       AUTHENTICATED     *
//  *                                                  *
//  ****************************************************

//Fata debug 
void listOnlineUsers(user_data_s *users, int maxUsers, int user){
    char buff[MAX_BUFF_SIZE] = {0};
    char cache[MAX_BUFF_SIZE] = {0};
    for(int i = 0; i < maxUsers; i++){
        if(users[i].fd != -1){
            snprintf(cache, sizeof(cache), "[%d] %s\n", users[i].id, users[i].username);
            strncat(buff, cache, sizeof(buff) - strlen(buff) - 1);
        }
    }
    printf("%s", buff);
    send(users[user].fd, buff, strlen(buff), MSG_NOSIGNAL);
    memset(buff, 0, sizeof(buff));
}
