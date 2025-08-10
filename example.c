// xThalach network server with select
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

// PROTOCOL HEADER

typedef enum{
	PROTO_HELLO,
}proto_type_e;

// TLV TypeLenValue
typedef struct{

	proto_type_e type;
	unsigned int len;

}proto_header_t; 



void headerCheck(int fd){

	char buff[1024] = {0};
	proto_header_t *hdr = (proto_header_t *)&buff;

	if(read(fd, hdr, sizeof(proto_header_t) + sizeof(int)) == -1){
		perror("read");
		close(fd);
		exit(1);
	}

	hdr->type = ntohs(hdr->type);
	hdr->len = ntohl(hdr->len);

	int *data = (int *)&hdr[1];

	if(hdr->type != PROTO_HELLO){
		if(write(fd, "Error en el tipo de protocolo!\n", 31) == -1){
			perror("write");
			close(fd);
			exit(1);
		}
		printf("Error en el tipo de protocolo!\n");
		exit(1);
	}

	if(*data != 1){		
		if(write(fd, "Error en la version del protocolo!\n", 35) == -1){
			perror("write");
			close(fd);
			exit(1);
		}
		printf("Error en la version del protocolo!\n");
		exit(1);
	}

	if(write(fd, "Se ha connectaco correctamente al servidor",43) == -1){
		perror("write");
		close(fd);
		exit(1);
	}

}

// Clientes
#define MAX_CLIENTS 256
#define BACKLOG 10
#define MAX_BUFF 1024
typedef enum{
	NEW,
	CONNECTED,
	DISCONNECTED,
}client_state_e;

typedef struct{

	int fd;
	client_state_e state;
	char buff[1024];
}clients;

clients clientes[MAX_CLIENTS] = {0};

// Inicializar los clientes para que el valor del file descriptor sea -1 indica que estan disponibles.
void inicializar_clientes(){

	for(int i=0; i < MAX_CLIENTS; i++){
		clientes[i].fd = -1;
		clientes[i].state = NEW;
		memset(&clientes[i].buff, 0, MAX_BUFF);
	}

}

// Funcion para buscar un cliente que este libre y devolver su identificador. 

int isFree(){
	
	for(int i=0; i < MAX_CLIENTS; i++){
		if(clientes[i].fd == -1){
			return i;
		}
	}
	return -1;
}

int main(){

	int listen_fd, conn_fd, nfds;
	fd_set readfds, writefds;

	struct sockaddr_in serverAddr = {0}, clientAddr = {0};
	socklen_t clientAddrLen = sizeof(struct sockaddr_in);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(9001);

	inicializar_clientes();

	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	if(bind(listen_fd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) == -1){
		perror("Bind");
		close(listen_fd);
		exit(EXIT_FAILURE);
	}

	if(listen(listen_fd, 10) == -1){
		perror("listen");
		close(listen_fd);
		exit(EXIT_FAILURE);

	}

	nfds = listen_fd + 1;
	int freeClient = 0;


	while(1){
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		
		
		FD_SET(listen_fd, &readfds);
		FD_SET(listen_fd, &writefds);

		for(int i=0; i < MAX_CLIENTS; i++){
			if(clientes[i].fd != -1){
				FD_SET(clientes[i].fd, &readfds);
				FD_SET(clientes[i].fd, &writefds);
				if(nfds <= clientes[i].fd) nfds = clientes[i].fd + 1;
			}
		}

		// select 
		if(select(nfds, &readfds, &writefds, NULL, NULL) == -1){
			perror("Select");
			close(listen_fd);
			exit(1);
		}

		if(FD_ISSET(listen_fd, &readfds)){
			if((conn_fd = accept(listen_fd, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1){
				perror("Accept");
				continue;
			}

			freeClient = isFree();
			if(freeClient == -1){
				printf("Todos los clientes estan ocupados!\n");
				close(listen_fd);
				close(conn_fd);
				exit(1);
			}else{
				clientes[freeClient].fd = conn_fd;
				clientes[freeClient].state = CONNECTED;
				if(nfds <= clientes[freeClient].fd) nfds = clientes[freeClient].fd + 1;
			}

		}


		for(int i=0; i < MAX_CLIENTS; i++){
			if(clientes[i].fd != -1 && FD_ISSET(clientes[i].fd, &readfds)){
				
				if(read(clientes[i].fd, clientes[i].buff, MAX_BUFF) <= 0){
					printf("Se ha desconectado");
					clientes[i].fd = -1;
					clientes[i].state = DISCONNECTED;
					memset(&clientes[i].buff, 0, MAX_BUFF);
					close(clientes[i].fd);
				}

				write(clientes[i].fd, clientes[i].buff, MAX_BUFF);
				memset(&clientes[i].buff, 0, MAX_BUFF);

			}
		}
		// headerCheck(conn_fd);

	}

	close(listen_fd);
	return 0;

}








