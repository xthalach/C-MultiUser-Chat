#ifndef PARSE_H
#define PARSE_H
 
#include "../include/common.h"



int freeUser(user_data_s *users ,fd_set *readfd);
void initialize(user_data_s *users);
void serverClose(user_data_s *users, fd_set *readfd);
void cleanUser(user_data_s *users, int fd);
void listUsers(user_data_s *users, int maxUsers);


#endif