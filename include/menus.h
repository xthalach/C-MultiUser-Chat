#ifndef MENUS_H
#define MENUS_H

#include "../include/common.h"

int login(db_header_s *db_header,user_data_s *users, int user, int clifd, char *tmpUser, char *tmpPass);
void newUserRegister(user_data_s *users, int i);
void exitChat(user_data_s *users, int i);
void userdetails(user_data_s *users, int maxUsers, int user);
void listOnlineUsers(user_data_s *users, int maxUsers, int user);
#endif