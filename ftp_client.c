#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int sendall(int sockThem, char *buffer, int fileLeng);

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

/*
 * TODO: Implement LIST, RETRIEVE, STORE, MULTITHREADING
 * Also fix the warnings
 */

    FILE *read;
    FILE *write;

    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char* quit;
    //bzero(quit, 256);
    quit = "QUIT\n";


    /*
     * Establishing Socket connection
     */
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /*
     * Loop to keep entering commands
     */
    while(1) {

        //writing to server
        printf("Please enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        //n = write(sockfd, buffer, strlen(buffer));
        if (sendall(sockfd, buffer, strlen(buffer)) == -1){
            fprintf(stderr, "unable tp sendall\n");
        }
        if (n < 0)
            error("ERROR writing to socket");

        //breaking out of while loop
        if(strcmp(buffer, quit) == 0){
            printf("Closing Connection\n");
            break;
        }

        //reading from server
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n", buffer);

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