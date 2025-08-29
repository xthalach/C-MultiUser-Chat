#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_USERS        10
#define SEND_FLAGS_DEFAULT 0
#define MAX_BUFF_SIZE   1024
#define BACKLOG         5
#define EXIT_ERROR      -1
#define USERNAME_SIZE   25
#define PASSWORD_SIZE   25
#define ROL_SIZE        25
#define MAGIC           "\x78\x54\x68\x61\x6c\x61\x63\x68" // xThalach

#define filename "newChat.db"

typedef enum{

    MENU,
    NEW,
    CONNECTED,
    DISCONECTED,
    PROTOTEST,
    AUTHENTICATED,
    
    
}user_stat_e;

typedef enum {
    
    USER,
    PASSWORD,
    AUTHNONE,    
    EDITUSER,
    EDITPASS,
    SELECTCHAT,
    OLDMSG,
    DELETEUSER,

}user_auth_stat_e;


typedef enum{

    LOGIN,
    REGISTER,
    FORGOT,
    EXIT,
    MENUNONE,
    
}user_menu_stat_e;

typedef struct {
    int fd;
    char buff[MAX_BUFF_SIZE];
} user_missing_chat_s;

typedef struct{

    int id; // Identificador Unic
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char rol[ROL_SIZE];

    int fd;
    char buff[MAX_BUFF_SIZE];
    int currentChat;
    user_missing_chat_s oldChat[MAX_USERS];
    int oldMsg[MAX_USERS]; 
    user_stat_e state;
    user_menu_stat_e menuState;
    user_auth_stat_e authState;
    
}user_data_s;


typedef struct {

    int dbLen; // 12 
    int usersLen; // 10 
    char magic[8]; 
      
}db_header_s;


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

int main(){


    db_header_s *db_header; 
    db_header = (db_header_s *)calloc(1, sizeof(db_header_s));

    user_data_s *users = (user_data_s *)calloc(MAX_USERS, sizeof(user_data_s));

    saveUsersData(db_header, users);

    printf("[!] New Database Created [!]\n");

    return 0; 
}