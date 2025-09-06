#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/select.h>

#include <errno.h>

#include "../include/functions.h"
#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/database.h"
#include "../include/menus.h"

int main(){

    int sockfd, nfds, clifd; 
    struct sockaddr_in sockAddress = {0}, clientAddress = {0};
    socklen_t clientAddressLen = sizeof(clientAddress);
    db_header_s *db_header = (db_header_s *)calloc(1, sizeof(db_header_s));
    
    
    user_data_s *users;
    printf("users after alloc %p\n", &users);
    //users = (user_data_s *)calloc(db_header->usersLen, sizeof(user_data_s));
    importDabase(db_header, &users);

    // printf("users before alloc %p\n", &users);
    
    // listUsers(users, db_header->usersLen);
    // printf("Free");
    // exit(1);
    

    fd_set readfd;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == EXIT_FAILURE){
        perror("socket");
        close(sockfd);
        return EXIT_FAILURE; 
    }

    sockAddress.sin_family = AF_INET;
    //sockAddress.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    sockAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sockAddress.sin_port = htons(9001); // 5555

    if(bind(sockfd, (struct sockaddr *)&sockAddress, sizeof(sockAddress)) == -1){
        perror("Bind\n");
        close(sockfd);
        return 1;
    }
   

    if(listen(sockfd, BACKLOG) == -1){
        perror("Listen\n");
        close(sockfd); 
        return -1;
    }
    
    nfds = sockfd + 1;

    
    initialize(users);
    
    while(1){
        FD_ZERO(&readfd);
        FD_SET(sockfd, &readfd);
        
        listUsers(users, db_header->usersLen);
        
        for(int i = 0; i < db_header->usersLen; i++){
            if(users[i].fd != -1){
                FD_SET(users[i].fd, &readfd);
            }
        }

        // // Reset the nfds variable to reuse the log off file descriptors from clients. This could make that another user gets a used file descriptor
        // for(int i = 0; i < db_header->usersLen; i++){
        //     if(users[i].fd != -1){
        //         if(nfds >= users[i].fd) nfds = users[i].fd + 1;
        //     }
        // }

        if(select(nfds, &readfd, NULL, NULL, NULL) == -1){
            fprintf(stderr, "[ERROR] SELECT: %s\n", strerror(errno));
            continue;
        }


        if(FD_ISSET(sockfd, &readfd)){
            if((clifd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1){
                perror("Accept\n");
                serverClose(users, &readfd);
                close(sockfd);
                return -1;
            }else{
                
                int user;
                if((user = freeUser(users ,&readfd)) == -1){
                    char usersFull[41] = "[!] Not enoght space the users are full!\n";
                    send(clifd, usersFull, strlen(usersFull), SEND_FLAGS_DEFAULT);
                    continue;
                }
                users[user].fd = clifd;
                users[user].state = PROTOTEST;
                users[user].authState = AUTHNONE;
                users[user].menuState = MENUNONE;
                //strcpy(users[user].username,"Testing\0");
                if(nfds <= clifd) nfds = clifd + 1;

                //handle_client(users[user].fd);
                // if(checkClientConnection(users[user].fd) == -1){
                //     close(users[user].fd);                    
                //     users[user].fd = -1;

                // }
            }
        }


        for(int i = 0; i < db_header->usersLen; i++){
            if(users[i].fd != -1 && FD_ISSET(users[i].fd, &readfd)){
                if(recv(users[i].fd,users[i].buff,MAX_BUFF_SIZE, SEND_FLAGS_DEFAULT) <= 0){
                    // No m'agrada el fet de que quant un usuari tanca la sesio es faci la copia, ja que li dones el poder al usuari per controlar-ho. 
                    // Molaria mes fer-ho quant el servidor es tanqui o implementar un temporitzador aleatori dintre de un rango de 3h / o cada 6h 
                    saveUsersData(db_header, users);
                    //listUsers(users);
                    cleanUser(users, i);                   
                    continue;
                }else{
                    if(users[i].state == PROTOTEST){
                        // Chapusa
                        if(checkClientConnection(users[i].fd, users, i) == 1){
                            close(users[i].fd);
                            users[i].fd = -1;
                        }else{
                            //  Login
                            // Welcome to the C multichat server
                            // [1] Login
                            // [2] Register
                            // [3] Forgot Account
                            // [4] Exit
                            send(users[i].fd,"Login\nWelcome to the C multichat server\n[1] Login\n[2] Register\n[3] Forgot Account\n[4] Exit\n",91,MSG_NOSIGNAL);
                            users[i].state = MENU;
                        }
                    }else if(users[i].state == MENU){
                        int menu = atoi(users[i].buff);
                        // El LOGIN + 1 es perque soc del mas de barberans okey?? Li vaig suma 1 perque no cuadraba amb el que l'usuari introduia. 
                        if( menu == LOGIN + 1 || users[i].menuState == LOGIN){
                            char tmpUser[USERNAME_SIZE];
                            char tmpPass[PASSWORD_SIZE];
                            if(login(db_header, users, i, clifd, tmpUser, tmpPass) == -1){
                                send(users[i].fd, "[!] Wrong Username or Password [!]\nLogin\nWelcome to the C multichat server\n[1] Login\n[2] Register\n[3] Forgot Account\n[4] Exit\n", 1024,MSG_NOSIGNAL);
                                menu = 0;
                                users[i].state == MENU;
                                users[i].menuState = MENUNONE;
                                users[i].authState = AUTHNONE;
                            }
                        }else if(menu == REGISTER + 1 || users[i].menuState == REGISTER) {
                            // Recordar check memoria dinamica
                            newUserRegister(users, i);
                        }else if(menu == EXIT + 1|| users[i].menuState == EXIT){
                            exitChat(users, i);
                        }else{
                            send(users[i].fd,"[ERROR] INCORRECT VALUE - USE THE MENU OPCIONS ONLY [ERROR]\nLogin\nWelcome to the C multichat server\n[1] Login\n[2] Register\n[3] Forgot Account\n",143,MSG_NOSIGNAL);
                        }

                    }else if(users[i].state == AUTHENTICATED){
                        // [1] List Online Users\n[2] Edit Username\n[3] Edit Password\n[4] Select User Chat\n[5] Show Waiting Room\n[6] Delete Account\n[7] Log Off\n
                        // Welcome nom del usuari <- En algun futur
                        // [1] List Online Users
                        // [2] Edit Username
                        // [3] Edit Password
                        // [4] Select User Chat
                        // [5] Show Waiting Room / Old Messages
                        // [6] Delete Account
                        // [7] Log Off
                        int menu = 0;
                        users[i].menuState = MENUNONE;
                        users[i].authState = AUTHNONE;
                        menu = atoi(users[i].buff);

                        if(menu == LISTUSERS - 1 || users[i].menuState == LISTUSERS){
                            send(users[i].fd, "[+] Inside List Users \n", 24, MSG_NOSIGNAL);
                            listOnlineUsers(users, db_header->usersLen, i);                            
                        }else if(menu == EDITUSER - 1 || users[i].menuState == EDITUSER){
                            send(users[i].fd, "[+] Inside Edit Username \n", 27, MSG_NOSIGNAL);
                        }


                    }else{
                        if(send(users[i].fd, users[i].buff, MAX_BUFF_SIZE, SEND_FLAGS_DEFAULT) == -1){
                            fprintf(stderr, "[ERROR] SEND currentChat: %s\n", strerror(errno));
                        }
                        memset(users[i].buff, 0, MAX_BUFF_SIZE); // Clean the user buffer.                         
                    }

                }
            }
        }
        

    }

    close(sockfd);

    return 0;
}
