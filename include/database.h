#ifndef DATABASE_H
#define DATABASE_H

#include "../include/common.h"


// Structure of the database header

// length + usersLen + firma 

typedef struct {

    int dbLen;
    int usersLen;
    char magic[10];
      
}db_header_s;


void saveUsersData(user_data_s *users);



#endif