// File that stores all users information into a file called chat.db

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include "../include/common.h"
#include "../include/database.h"

#define filename "chat.db"

void importDabase(){

}

void saveUsersData(user_data_s *users){

    FILE *database; 

    database = fopen(filename, "w");

    fwrite(users, sizeof(user_data_s), MAX_USERS, database);

    fclose(database);
    
}

void login(user_data_s *users, int i){
    users[i].menuState = LOGIN;



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
        send(users[i].fd, users[i].username, USERNAME_SIZE,MSG_NOSIGNAL);
        send(users[i].fd, users[i].password, PASSWORD_SIZE, MSG_NOSIGNAL);
        // S'han de posar per defecte les variables  
        users[i].menuState = 0;
        users[i].authState = 0; 
        users[i].state = AUTHENTICATED;
    }
    
    //send(users[i].fd, "Inside register funtion\n", 25, MSG_NOSIGNAL);
}