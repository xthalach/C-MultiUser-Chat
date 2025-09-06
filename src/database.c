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

// Funcio per importar la base de dades i fer el checks corresponents por comprobar si la bbdd es legitima. 
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

// Guardar la informacio dels clients a la bbdd amb les seves metadades del header.
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

