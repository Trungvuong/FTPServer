#include<sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void* commands (void* arg);

int main(){
    socklen_t  addr_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage incomingAddr;

    int yes = 1;
    int sock;
    int* incomingSock;
    int status;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *loop;

    pthread_t newThread;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*
     * Sets the ip address in the first arg (NULL leaves it unchanged from hints
     * Sets the port number in the second arg (both first and second are strings)
     * Uses the values set in hints
     * Stores a linked link at servinfo
     */
    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    /*
     * Loops through all nodes from getaddrinfo until a connection is found
     * Returns a socket or -1 on error
     * ai_family is the ip type
     * ai_socktype is the socket type
     * ai_protocol is the protocol to be used with the socket (0 if only one protocol exists)
     */
    for(loop = servinfo; loop != NULL; loop->ai_next){

        /*
         *  Returns a socket or -1 on error
         * ai_family is the ip type
         * ai_socktype is the socket type
         * ai_protocol is the protocol to be used with the socket (0 if only one protocol exists)
         */
        if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0){
            fprintf(stderr, "socket error");
            continue;
        }

        /*
         * Frees the socket if already in use
         * returns -1 on error
         */
        if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
            perror("setsockopt");
            continue;
        }

        /*
         * Bind the socket sock to a specific port number
         * returns -1 on error
         */
        if((bind(sock, servinfo->ai_addr, servinfo->ai_addrlen)) < 0){
            fprintf(stderr, "bind error");
            continue;
        }
        break;
    }

    /*
     * socket has been created no longer need addrinfo
     */
    freeaddrinfo(servinfo);

    /*
     * all nodes checked with no connection
     */
    if(loop == NULL){
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    /*
     * Creates a queue of incoming connection from sock
     * Max length of 10
     */
    if((listen(sock, 10)) < 0){
        fprintf(stderr, "listen error");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    /*
     * Returns a new socket connected from the queue of listen
     * malloc to prevent overwrite while multi-threading
     */
    while(1){
        incomingSock = malloc(sizeof(int));
        if((*incomingSock = accept(sock, (struct sockaddr *) &incomingAddr, &addr_size) < 0)){
            fprintf(stderr, "accept error");
            continue;
        }
        /*
         * Creates a new thread and passes the incomingSock as the arg
         */
        if ((status = pthread_create (&newThread, NULL, commands, incomingSock)) != 0){
            fprintf (stderr, "thread create error %d: %s \n", status, strerror(status));
            free(incomingSock);
            exit(1);
        }
    }
}

void* commands(void* clientSocket){
    char buf[500];
    int byteRecv;
    if(byteRecv = recv(*(int*)clientSocket, buf, 500, 0) == 0){
        close(*(int*)clientSocket);
    } else if(byteRecv < 0){
        perror("recv");
        exit(1);
    }
}
