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

void saveUsersData(user_data_s *users, db_header_s *dbHeader){

    FILE *database; 

    database = fopen(filename, "w");

    fwrite()

    fwrite(users, sizeof(user_data_s), MAX_USERS, database);

    fclose(database);
    
}