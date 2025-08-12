// common.h
#ifndef COMMON_H
#define COMMON_H

#define MAX_USERS        250
#define SEND_FLAGS_DEFAULT 0
#define MAX_BUFF_SIZE   1024
#define BACKLOG         5
#define EXIT_ERROR      -1
#define USERNAME_SIZE   25
#define PASSWORD_SIZE   25
#define ROL_SIZE        25
#define MAGIC[8]       "\x78\x54\x68\x61\x6c\x61\x63\x68" // xThalach

// Clients structure 
typedef enum{

    NEW,
    CONNECTED,
    DISCONECTED,
    PROTOTEST,

}user_stat_e;

typedef enum{
    LOGIN,
    REGISTER,
    FORGOT,
    USER,
    PASSWORD,

}user_auth_stat_e;

typedef struct{

    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char rol[ROL_SIZE];

    int fd;
    char buff[MAX_BUFF_SIZE];
    int currentChat;
    user_stat_e state;
    
}user_data_s;


#endif // COMMON_H
