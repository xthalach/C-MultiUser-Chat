#ifndef DATABASE_H
#define DATABASE_H

#include "../include/common.h"


// Structure of the database header

// length + usersLen + firma 

typedef struct {

    int dbLen; // 12 
    int usersLen; // 10 
    char magic[9]; 
      
}db_header_s;


void saveUsersData(user_data_s *users);
void login(user_data_s *users, int i);
void newUserRegister(user_data_s *users, int i);


#endif