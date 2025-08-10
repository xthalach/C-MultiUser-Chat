#ifndef PARSE_H
#define PARSE_H
 
#include "../include/common.h"
#include <sys/select.h>


int freeUser(user_data_s **users ,fd_set *readfd);
void initialize(user_data_s **users);
void serverClose(user_data_s **users, fd_set *readfd);
void cleanUser(user_data_s **users, int fd);
void userRegister();
void proba();

#endif