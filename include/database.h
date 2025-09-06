#ifndef DATABASE_H
#define DATABASE_H

#include "../include/common.h"


// Structure of the database header

// length + usersLen + firma 

typedef struct {

    int dbLen; // 12 
    int usersLen; // 10 
    char magic[8]; 
      
}db_header_s;

void importDabase(db_header_s *db_header, user_data_s **users);
void saveUsersData(db_header_s *db_header, user_data_s *users);


#endif