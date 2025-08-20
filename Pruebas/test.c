#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define MAX_USERS 2

enum rol{
    ADMIN,
    USER,
};


struct Users {
    char username[25];
    char password[25];
    enum rol priv;
};


void dinamic(struct Users *users){

    printf("inside: %p\n", (void *)users);

    //struct Users *tmp; 

    users = (struct Users *)calloc(2, sizeof(struct Users));
  
}

void print(){
    printf("dentro");
}

void change(struct Users *users){

    printf("inside pointer: %p\n", users);
    strcpy(users[1].username, "Prueba");
    strcpy(users[1].password, "Testing");

}

int main(){

    int age = 5;
    struct Users *users;
 
    printf("outside: %p\n", (void *)users);
    dinamic(users);
    printf("after: %p\n", (void *)users);

    strcpy(users[0].username, "xThalach");
    strcpy(users[0].password, "Password");

    printf("outside pointer: %p\n", users);
    change(users);
    

    if(age == 10){
        
        printf("age: %d\n", age);
    }



    for(int i = 0; i < MAX_USERS; i++){
        printf("[%d] Username: %s\n", i, users[i].username);
        printf("[%d] Password: %s\n", i, users[i].password);
    }

    free(users);
}