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

// Clients structure 
typedef enum{

    NEW,
    CONNECTED,
    DISCONECTED,

}user_stat_e;


typedef struct{

    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];

    int fd;
    char buff[MAX_BUFF_SIZE];
    int currentChat;
    user_stat_e state;
    
}user_data_s;


#endif // COMMON_H
