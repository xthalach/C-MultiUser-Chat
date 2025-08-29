// File that stores all users information into a file called chat.db

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

#define filename "chat.db"

void importDabase(db_header_s *db_header, user_data_s **users){

    FILE *database;
    int fd;

    struct stat *stat;

    database = fopen(filename, "rb");
    
    if(database == NULL){
        fprintf(stderr, "[ERROR] DB: %s\n", strerror(errno));
        exit(1);    
    }

    fd = fileno(database);

    fstat(fd, stat);

    fread(db_header, sizeof(db_header_s), 1, database);

    printf("Size header struct: %ld\n", sizeof(db_header));
    printf("header size: %d\nstat size: %ld\n",db_header->dbLen, stat->st_size);
    if(db_header->dbLen != stat->st_size){
        printf("WRONG DB SIZE\n");
        exit(1);
    }

    int userLen = (db_header->dbLen - sizeof(db_header_s)) / sizeof(user_data_s);
    printf("header userLen: %d\nstat userlen: %d\n",db_header->usersLen, userLen); 
    if(db_header->usersLen != userLen){
        printf("WRONG USER LEN\n");
        exit(1);
    }

    printf("header magic: %s\nmagic: %s\n",db_header->magic, MAGIC);
    if(strcmp(db_header->magic, MAGIC) != 0){
        
        printf("WRONG MAGIC WORD\n");
        exit(1);
    }
    
    
    *users = (user_data_s *)calloc(db_header->usersLen, sizeof(user_data_s));
    
    fread(*users, sizeof(user_data_s),db_header->usersLen, database );
    
    fclose(database);
    
}

void saveUsersData(db_header_s *db_header, user_data_s *users){

    FILE *database; 

    database = fopen(filename, "wb");

    db_header->dbLen = sizeof(db_header_s) + (sizeof(user_data_s) * MAX_USERS);
    printf("header len: %d\n", db_header->dbLen); 
    db_header->usersLen = (sizeof(user_data_s) * MAX_USERS) / sizeof(user_data_s);
    printf("db users: %d\n", db_header->usersLen);
    strcpy(db_header->magic, MAGIC);

    fwrite(db_header, sizeof(db_header_s), 1 ,database);
    fwrite(users, sizeof(user_data_s), MAX_USERS, database);

    fclose(database);
    
}

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
                    users[i].menuState = 0;
                    users[i].authState = 0; 
                    users[i].state = AUTHENTICATED;
                    users[user].fd = -1; 
                    strcpy(tmpUser, "");
                    strcpy(tmpPass, "");
                    send(users[i].fd, "[+] Authenticated [+]\n", 22, MSG_NOSIGNAL);
                    send(users[i].fd, "[+] USER MENU\n[1] List Online Users\n[2] Edit Username\n[3] Edit Password\n[4] Delete Account\n", 91, MSG_NOSIGNAL);
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
        
        //send(users[i].fd, users[i].password, PASSWORD_SIZE, MSG_NOSIGNAL);
        // S'han de posar per defecte les variables  
        users[i].menuState = 0;
        users[i].authState = 0; 
        users[i].state = AUTHENTICATED;
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