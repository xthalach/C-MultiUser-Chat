#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



typedef struct{
    char buff[1024];
    int fd;
}users;


int main(){


    users *usuarios;

    usuarios = calloc(10, sizeof(users));



    /// Users 1
    
    usuarios[0].fd = 5;


    // Users 2 

    usuarios[1].fd = 10;


    printf("users1: %d\n", usuarios[0].fd);
    
    // Array
    
    users test[5];


    test[0].fd = 67;

    test[1].fd = 99;   

    printf("user2 test: %d\n", test[1].fd);

    free(usuarios);
    return 0;
}