/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

int sendall(int sockThem, char *buffer, int fileLeng);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

/*
 * TODO: Implement LIST, RETRIEVE, STORE, MULTITHREADING
 * Also fix the warnings
 */

    int sockfd, newsockfd, portno, clilen;

    //variable to transfer files
    char buffer[256];
    char* quit;
    //bzero(quit, 256);
    quit = "QUIT\n";
    FILE *read;
    FILE * write;

    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");



    while(1) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");

        printf("Here is the message: %s\n", buffer);

        if(strcmp(buffer, quit) == 0){
            printf("Shutting Down\n");
            break;
        }

        n = write(newsockfd, "I got your message", 18);
        if (n < 0)
            error("ERROR writing to socket");

    }
    return 0;
}

int sendall(int sockThem, char *buffer, int fileLeng){
    int total = 0; //how many bytes sent
    int bytesLeft = fileLeng; // how many bytes left to send
    int bytesSent; //how many bytes sent

    while(total < fileLeng){
        bytesSent = write(sockThem, buffer + total, bytesLeft);
        if (bytesSent == -1){
            fprintf(stderr, "Sending error\n");
            return -1;
        }
        total += bytesSent;
        bytesLeft -= bytesSent;
    }
    return total;
}